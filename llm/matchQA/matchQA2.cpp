#include "llama.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <numeric>

// Structure to hold our question data
struct QuestionEmbedding {
    int id_number;
    std::string question_text;
    std::vector<float> embedding;
};

// Function to calculate cosine similarity between two vectors
float cosine_similarity(const std::vector<float>& v1, const std::vector<float>& v2) {
    if (v1.size() != v2.size() || v1.empty()) return 0.0f;
    float dot_product = 0.0f;
    float norm_v1 = 0.0f;
    float norm_v2 = 0.0f;
    for (size_t i = 0; i < v1.size(); ++i) {
        dot_product += v1[i] * v2[i];
        norm_v1 += v1[i] * v1[i];
        norm_v2 += v2[i] * v2[i];
    }
    if (norm_v1 == 0.0f || norm_v2 == 0.0f) return 0.0f;
    return dot_product / (std::sqrt(norm_v1) * std::sqrt(norm_v2));
}

// Function to generate an embedding using llama.cpp
std::vector<float> generate_embedding(llama_context* ctx, const std::string& text) {

    // Assuming 'ctx' is your valid llama_context* pointer
    // Assuming 'text' is your std::string holding the input question

    // 1. Get the model from the context
    const llama_model* model = llama_get_model(ctx);

    // 2. Get the vocab from the model
    const llama_vocab* vocab = llama_get_vocab(model); 

    // 3. Prepare the necessary inputs for the full function call
    std::vector<llama_token> tokens(1024); // Allocate a buffer for tokens (adjust size as needed)
    const char* text_cstr = text.c_str();
    int text_len = text.length();
    int n_tokens_max = tokens.size();
    bool add_special = true; // Equivalent to add_bos
    bool parse_special = false; // Typically set to false for general tokenization

    // 4. Call the function using the correct signature
    int32_t n_tokens = llama_tokenize(
        vocab,           // const struct llama_vocab * vocab
        text_cstr,       // const char * text
        text_len,        // int32_t text_len
        tokens.data(),   // llama_token * tokens (pointer to the start of our vector buffer)
        n_tokens_max,    // int32_t n_tokens_max
        add_special,     // bool add_special
        parse_special    // bool parse_special
    );

    // 5. Handle potential errors and resize the vector to the actual number of tokens found
    if (n_tokens <= 0) {
        // Handle error (e.g., buffer too small or bad input)
        std::cerr << "Failed to tokenize text or buffer too small." << std::endl;
        tokens.clear();
    } else {
        tokens.resize(n_tokens); // Resize vector to actual token count
    }

    // Ensure context capacity is enough
    if (tokens.size() > llama_n_ctx(ctx)) {
        std::cerr << "Prompt too long for context size" << std::endl;
        return {};
    }

//  struct llama_batch llama_batch_get_one(
//               llama_token * tokens,
//                   int32_t   n_tokens) {
//      return {
//          /*n_tokens =*/ n_tokens,
//          /*tokens   =*/ tokens,
//          /*embd     =*/ nullptr,
//          /*pos      =*/ nullptr,
//          /*n_seq_id =*/ nullptr,
//          /*seq_id   =*/ nullptr,
//          /*logits   =*/ nullptr,
//      };
//  }

    // Pass tokens to the model for embedding generation
    if (llama_decode(ctx, llama_batch_get_one(
                                                tokens.data(),
                                                tokens.size()
                                             )
                    )
        )
        { std::cerr << "Failed to decode tokens" << std::endl;
        return {};
    }

    // Get the embedding vector. This requires the model to have been loaded with 
    // `llm_params.embedding = true`
    const float* embedding_data = llama_get_embeddings(ctx);
    if (!embedding_data) {
        std::cerr << "Failed to get embeddings. Ensure the model supports it and llama_decode was successful." << std::endl;
        return {};
    }

    int embd_size = llama_model_n_embd(model);

    return std::vector<float>(embedding_data, embedding_data + embd_size);
}

// Function to save embeddings to a binary file
void save_embeddings_to_binary(const std::vector<QuestionEmbedding>& db, const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    int num_embeddings = db.size();
    ofs.write(reinterpret_cast<const char*>(&num_embeddings), sizeof(num_embeddings));

    if (num_embeddings > 0) {
        int embd_size = db[0].embedding.size();
        ofs.write(reinterpret_cast<const char*>(&embd_size), sizeof(embd_size));

        for (const auto& q : db) {
            ofs.write(reinterpret_cast<const char*>(&q.id_number), sizeof(q.id_number));
            // Save raw embedding data
            ofs.write(reinterpret_cast<const char*>(q.embedding.data()), q.embedding.size() * sizeof(float));
        }
    }
    ofs.close();
    std::cout << "Saved " << num_embeddings << " embeddings to " << filename << std::endl;
}

// Function to load embeddings from a binary file
std::vector<QuestionEmbedding> load_embeddings_from_binary(const std::string& filename) {
    std::vector<QuestionEmbedding> db;
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open()) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return db;
    }

    int num_embeddings = 0;
    ifs.read(reinterpret_cast<char*>(&num_embeddings), sizeof(num_embeddings));

    if (num_embeddings > 0) {
        int embd_size = 0;
        ifs.read(reinterpret_cast<char*>(&embd_size), sizeof(embd_size));
        
        for (int i = 0; i < num_embeddings; ++i) {
            QuestionEmbedding q;
            ifs.read(reinterpret_cast<char*>(&q.id_number), sizeof(q.id_number));
            q.embedding.resize(embd_size);
            ifs.read(reinterpret_cast<char*>(q.embedding.data()), embd_size * sizeof(float));
            db.push_back(std::move(q)); // Move the object into the vector
        }
    }
    ifs.close();
    std::cout << "Loaded " << num_embeddings << " embeddings from " << filename << std::endl;
    return db;
}


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << "../models/snowflake-arctic-embed-m.i1-Q4_K_M.gguf" << std::endl;
        return 1;
    }
    const std::string model_path = argv[1];
    const std::string db_file = "question_embeddings.bin";
    
    // Initialize llama.cpp
    llama_backend_init(false); // use_mlock

    llama_model_params model_params = llama_model_params_from_gpt_params(gpt_params{});
    llama_context_params ctx_params = llama_context_params_from_gpt_params(gpt_params{});
    ctx_params.embedding = true; // Crucial for embedding models

    llama_model* model = llama_load_model_from_file(model_path.c_str(), model_params);
    if (!model) {
        std::cerr << "Failed to load model" << std::endl;
        return 1;
    }
    llama_context* ctx = llama_new_context_with_model(model, ctx_params);
    if (!ctx) {
        std::cerr << "Failed to create context" << std::endl;
        return 1;
    }

    // --- 1. Populate/Generate Embeddings (Run once to create the binary file) ---
    std::vector<QuestionEmbedding> db_questions = {
        {101, "What is the capital of France?", {}},
        {102, "How does photosynthesis work?", {}},
        {103, "Who invented the telephone?", {}},
        {104, "What are the benefits of C++ programming?", {}}
    };

    for (auto& q : db_questions) {
        q.embedding = generate_embedding(ctx, q.question_text);
    }

    save_embeddings_to_binary(db_questions, db_file);


    // --- 2. Load Embeddings for Search (What your final app would do) ---
    std::vector<QuestionEmbedding> loaded_db = load_embeddings_from_binary(db_file);

    // --- 3. Process a New Question and Find the Best Match ---
    std::string new_question = "How to use C++?";
    std::vector<float> query_embedding = generate_embedding(ctx, new_question);

    int best_match_id = -1;
    float max_similarity = -1.0f;

    for (const auto& q : loaded_db) {
        float similarity = cosine_similarity(query_embedding, q.embedding);
        if (similarity > max_similarity) {
            max_similarity = similarity;
            best_match_id = q.id_number;
        }
    }

    std::cout << "\nNew Question: \"" << new_question << "\"" << std::endl;
    std::cout << "Best Match ID: " << best_match_id << std::endl;
    std::cout << "Similarity Score: " << std::fixed << std::setprecision(4) << max_similarity << std::endl;

    // Cleanup
    llama_free(ctx);
    llama_free_model(model);
    llama_backend_free();

    return 0;
}