// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | match_qa.cpp                                                |
//| EntryPoint   | main                                                        |
//| Purpose      | Find best match of new_questions with database questions    |
//| Inputs       | new_questions.csv, database_questions_answers.csv           |
//| Outputs      | results.csv                                                 |
//| Dependencies | llama.cpp                                                   |
//| By Name,Date | T.Sciple, 11/26/2025                                        |

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <cmath>
#include "../../llama.cpp/include/llama.h"

struct ResultRecord {
    std::string seq_indx_str;
    std::string new_question;
    std::string matched_db_question;
    std::string matched_db_answer;
    float score;
};


struct QAEmbed {
    std::string db_question;
    std::string db_answer;
    std::vector<float> embedding;
};


class Matcher {
    llama_model*   model = nullptr;
    const llama_vocab* vocab = nullptr;
    llama_context* ctx   = nullptr;

    // This function takes in two differect normalized vector embeddings and computes the
    // dot product for each dimension - see 'find_closest_match' function for a simplified example
    // of this.  Note that the vectors computed for this model have 384 dimensions
    float dot_product(const std::vector<float>& a, const std::vector<float>& b) const {
        if (a.size() != b.size()) return 0.0f;
        float dot = 0.0f;

        // Loop thru each dimension and compute the dot product 
        for (size_t i = 0; i < a.size(); ++i) {
            dot += a[i] * b[i];
        }
        return dot;
    }


    void print_embedding(const std::vector<float>& emb, const std::string& label) {
        std::cout << "Embedding for " << label << ": [";
        size_t n = emb.size();
        for(size_t i = 0; i < 10 && i < n; ++i) std::cout << emb[i] << " ";
        if (n > 20) std::cout << "... ";
        for(size_t i = n - 10; i < n; ++i) if (i >= 10) std::cout << emb[i] << " ";
        std::cout << "]" << std::endl;
    }

    // Add this new static helper function here:
    static std::vector<std::string> split_pipe_csv_line(const std::string& line) {
        std::vector<std::string> result;
        std::string field;
        bool in_quotes = false;

        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];

            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    field += '"';
                    ++i;  // skip the second quote
                } else {
                    in_quotes = !in_quotes;
                }
            }
            else if (c == '|' && !in_quotes) {
                result.push_back(field);
                field.clear();
            }
            else {
                field += c;
            }
        }
        result.push_back(field);  // last field
        return result;
    }

public:
    Matcher(const std::string& path) {
        llama_backend_init();
        
        llama_model_params mparams = llama_model_default_params();
        model = llama_model_load_from_file(path.c_str(), mparams);
        if (!model) throw std::runtime_error("Failed to load model");

        vocab = llama_model_get_vocab(model);
        
        if (!vocab) {
            std::cerr << "Vocab is null. Model may not be compatible.\n";
            return; // or handle it however you need
        }

        llama_context_params cparams = llama_context_default_params();
        cparams.n_ctx = 512;
        cparams.embeddings = true;

        ctx = llama_init_from_model(model, cparams);
        if (!ctx) throw std::runtime_error("Context init failed");
        // std::cout << "Model loaded. Embedding dimension: " << llama_model_n_embd(model) << "\n";
    }

    ~Matcher() {
        if (ctx)   llama_free(ctx);
        if (model) llama_model_free(model);
        llama_backend_free();
    }


    std::vector<float> embed(const std::string &text) {
        llama_context_params cparams = llama_context_default_params();
        cparams.n_ctx = 512;
        cparams.embeddings = true;
        cparams.pooling_type = LLAMA_POOLING_TYPE_NONE;

        llama_context* temp_ctx = llama_init_from_model(model, cparams);
        if (!temp_ctx) {
            std::cerr << "Failed to create temporary context for: " << text << "\n";
            return {};
        }

        const std::string prefix = "Represent this sentence for searching: ";
        std::string full_text = prefix + text;

        std::vector<llama_token> tokens(512);        
        int32_t n_tokens = llama_tokenize(
            vocab,
            full_text.c_str(),  
            static_cast<int32_t>(full_text.size()),
            tokens.data(),
            static_cast<int32_t>(tokens.size()),
            true,   // add BOS
            false   // don't special-handle EOT
        );
    
        if (n_tokens < 0) {
            std::cerr << "Tokenization failed or overflowed for: " << text << " (needed " << -n_tokens << " tokens)\n";
            llama_free(temp_ctx);
            return {};
        }

        llama_batch batch = llama_batch_get_one(tokens.data(), n_tokens);
        if (llama_decode(temp_ctx, batch) != 0) {
            llama_free(temp_ctx);
            return {};
        }

        int dim = llama_model_n_embd(model);
        
        std::vector<float> mean_emb(dim, 0.0f);
        for (int32_t i = 0; i < n_tokens; ++i) {
            const float* emb = llama_get_embeddings_ith(temp_ctx, i);
            if (!emb) {
                std::cerr << "No embedding for token " << i << " in: " << full_text << "\n";
                llama_free(temp_ctx);
                return {};
            }
            for (int j = 0; j < dim; ++j) {
                mean_emb[j] += emb[j];
            }
        }
        for (int j = 0; j < dim; ++j) {
            mean_emb[j] /= n_tokens;
        }

        float norm = 0.0f;
        for (float v : mean_emb) norm += v*v;
        norm = std::sqrt(norm);
        if (norm > 0.0f)
            for (auto &v : mean_emb) v /= norm;

        llama_free(temp_ctx);
        return mean_emb;
    }


    std::vector<QAEmbed> load(const std::string& csv_path) {
        std::vector<QAEmbed> db_qa;
        std::ifstream f(csv_path);
        if (!f.is_open()) {
            std::cerr << "Error: Could not open " << csv_path << "\n";
            return db_qa;
        }

        std::string line;
        if (!std::getline(f, line)) return db_qa; // skip header

        while (std::getline(f, line)) {
            if (line.empty()) continue;

            auto fields = split_pipe_csv_line(line);

            // Expect at least 2 fields: question | answer
            if (fields.size() < 2) {
                std::cerr << "Warning: Skipping malformed line: " << line << "\n";
                continue;
            }

            std::string db_q = fields[0];
            std::string db_a = fields[1];

            auto e = embed(db_q);
            if (!e.empty()) {
                db_qa.push_back({db_q, db_a, std::move(e)});
            }
        }
        return db_qa;
    }


    std::tuple<std::string, std::string, float> find_closest_match(const std::string& new_q,
                                       const std::vector<QAEmbed>& db_qa) {
        
        // Get embedding for the new question
        auto new_q_embed = embed(new_q);
        if (new_q_embed.empty() || db_qa.empty()) return {"No match", "No match", 0.0f};
        // print_embedding(new_q_embed, "New: " + new_q);

        float best = -1.0f;
        std::string ans;
        std::string match_q;
        // std::cout << "Similarities for new question '" << new_q << "':" << std::endl;
        
        // the following loop iterates thru each database question called item and computes
        // the dot product with each of the normalized vectors databased question and the new question
        // the dot product that is closest to 1 will be the vector that has the closest matching
        // dimensional values.
        // vector a     [ 0.20, 0.25, 0.947364766 ]
        // vector b     [ 0.25, 0.30, 0.920597632 ]
        // dot product  [ 0.050 + 0.075 + 0.87214176 ] = 0.9971  '<  very close match
        for (const auto& item : db_qa) {
            float s = dot_product(new_q_embed, item.embedding);
            // std::cout << "  to '" << item.db_question << "': " << s << std::endl;
            if (s > best) { best = s; ans = item.db_answer; match_q = item.db_question; }
        }
        return {match_q, ans, best};
    }
};


std::vector<std::string> load_questions_from_csv(const std::string &filename) {
    std::vector<std::string> questions;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << "\n";
        return questions;
    }

    std::string line;
    if (!std::getline(file, line)) return questions; // skip header
    
    while (std::getline(file, line)) {
        if (!line.empty()) {
            questions.push_back(line);
        }
    }

    return questions; 
}


int write_vec_db_to_binary_file(std::filesystem::file_time_type last_csv_write_time, const std::string &filename, const std::vector<QAEmbed> &db) {

    std::ofstream vec_db (filename, std::ios::binary);
    if (!vec_db.is_open()) {
        std::cerr << "Error: Could not open " << filename << " for writing\n";
        return -1;
    } 

    // write the date/time stamp to the binary file
    vec_db.write(reinterpret_cast<const char*>(&last_csv_write_time), sizeof(std::time_t));
    
    for (const auto &item : db) {
        size_t q_size = item.db_question.size();
        size_t a_size = item.db_answer.size();
        size_t emb_size = item.embedding.size();

        vec_db.write(reinterpret_cast<const char*>(&q_size), sizeof(size_t));
        vec_db.write(item.db_question.c_str(), q_size);

        vec_db.write(reinterpret_cast<const char*>(&a_size), sizeof(size_t));
        vec_db.write(item.db_answer.c_str(), a_size);

        vec_db.write(reinterpret_cast<const char*>(&emb_size), sizeof(size_t));
        vec_db.write(reinterpret_cast<const char*>(item.embedding.data()), emb_size * sizeof(float));
    }
    vec_db.close();
    return 0;
}


int read_vec_db_from_binary_file(std::filesystem::file_time_type , const std::string &filename, std::vector<QAEmbed> &db) {
    std::ifstream vec_db(filename, std::ios::binary);
    if (!vec_db.is_open()) {
        std::cerr << "Error: Could not open " << filename << " for reading\n";
        return -1;
    }

    // discard timestamp
    std::time_t ts = 0;
    if (!vec_db.read(reinterpret_cast<char*>(&ts), sizeof(std::time_t))) {
        std::cerr << "Error: Failed to read timestamp from " << filename << "\n";
        return -1;
    }

    while (true) {
        size_t q_size = 0;
        if (!vec_db.read(reinterpret_cast<char*>(&q_size), sizeof(size_t))) break; // EOF reached cleanly

        std::string q;
        q.resize(q_size);
        if (q_size > 0 && !vec_db.read(&q[0], q_size)) { std::cerr << "Error reading question\n"; return -1; }

        size_t a_size = 0;
        if (!vec_db.read(reinterpret_cast<char*>(&a_size), sizeof(size_t))) { std::cerr << "Unexpected EOF reading answer size\n"; return -1; }
        std::string a;
        a.resize(a_size);
        if (a_size > 0 && !vec_db.read(&a[0], a_size)) { std::cerr << "Error reading answer\n"; return -1; }

        size_t emb_size = 0;
        if (!vec_db.read(reinterpret_cast<char*>(&emb_size), sizeof(size_t))) { std::cerr << "Unexpected EOF reading embedding size\n"; return -1; }
        std::vector<float> emb(emb_size);
        if (emb_size > 0 && !vec_db.read(reinterpret_cast<char*>(emb.data()), emb_size * sizeof(float))) { std::cerr << "Error reading embedding\n"; return -1; }

        db.push_back({ std::move(q), std::move(a), std::move(emb) });
    }

    return 0;
}


int main(int argc, char** argv) {
    (void)argc;  // Suppresses the warning without side effects
    try {

        // Read current executable path used with building two paths below
        std::filesystem::path exe_dir_par = std::filesystem::path(argv[0]).parent_path();
        exe_dir_par = exe_dir_par.parent_path();

        // Pass the gguf file to the Matcher class.  Note that this particulate model
        // is used for sentence matching
        Matcher m("C:/dev/cpp/llm/models/all-minilm-l6-v2-q4_0.gguf");
        
        // Check to see if the binary vector database file exists and is up to date
        std::filesystem::path vec_db_path = exe_dir_par / "data/vec_dase.bin";
        std::string vec_db_path_str = vec_db_path.string();
        std::filesystem::path db_csv_path = exe_dir_par / "data/database_questions_answers.csv";
        std::string db_csv_path_str = db_csv_path.string();
        bool load_from_binary = false;

        auto db_csv_time = std::filesystem::last_write_time(db_csv_path);
        if (std::filesystem::exists(vec_db_path)) {
            auto vec_db_time = std::filesystem::last_write_time(vec_db_path);
            if (vec_db_time >= db_csv_time) {
                load_from_binary = true;
            }
        }

        // Either load the vector database from the binary file or from the csv file

        std::vector<QAEmbed> db;
        // 1. Binary Option:
        if (load_from_binary) {
            int status = read_vec_db_from_binary_file(db_csv_time, vec_db_path_str, db);
            if (status != 0) {
                std::cerr << "Warning: Failed to read vector database from binary file. Loading from CSV instead.\n";
                load_from_binary = false;
            } 
        // 2. CSV and Re-Embedding Option:
        } else {
            // Load the questions and answers from the csv file and create the vector database
            db = m.load(db_csv_path_str);
            
            // write the vector database to a binary file for faster loading next time
            std::filesystem::path vec_db_path = exe_dir_par / "data/vec_dase.bin";
            std::string vec_db_path_str = vec_db_path.string();
            int status = write_vec_db_to_binary_file(db_csv_time, vec_db_path_str, db);
            if (status != 0) {
                std::cerr << "Warning: Failed to write vector database to binary file.\n";
            }
        }

        // Set path to current executable path and read in new questions from csv file
        std::filesystem::path new_questions_filepath = exe_dir_par / "data/new_questions.csv";
        std::string new_questions_filepath_str = new_questions_filepath.string();
        std::vector<std::string> new_questions = load_questions_from_csv(new_questions_filepath_str);
        
        std::vector<ResultRecord> results;
        int seq_indx =1;
        std::string seq_indx_str;
        for (const auto& new_q : new_questions) {
            // Get the matched question (mq), answer (a), and score (sc)
            auto [mq, ans, scr] = m.find_closest_match(new_q, db);
            seq_indx_str = std::to_string(seq_indx) + ".";
            results.push_back({seq_indx_str, new_q, mq, ans, scr});
            seq_indx++;
        }

        std::filesystem::path out_path = exe_dir_par / "results/results.csv";

        std::ofstream out(out_path);
        out << "Seq|New_Question|Matched_Db_Question|Db_Answer|Score\n";

        for (auto& r : results) {
            out << '"' << r.seq_indx_str << "\"|"
                << '"' << r.new_question << "\"|"
                << '"' << r.matched_db_question << "\"|"
                << '"' << r.matched_db_answer << "\"|"
                << r.score << "\n";
                seq_indx++;
        }
        out.close();
        // wait until the end to indicate message are printed
        if (load_from_binary) {
            std::cout << "Loaded vector database from binary file.\n";
        } else {
            std::cout << "Loaded vector database from CSV file\n"
                        << "Vector database written to binary file\n";
        }
        std::wcout << L"Results written to: " << out_path.wstring() << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    
    system ("pause");
    return 0;
}                                                                                                                                                         