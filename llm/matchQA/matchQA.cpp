#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include "../llama.cpp/include/llama.h"

struct QAPair {
    std::string question, answer;
    std::vector<float> embedding;
};

class Matcher {
    llama_model*   model = nullptr;
    const llama_vocab* vocab = nullptr;
    llama_context* ctx   = nullptr;


    float cos_sim(const std::vector<float>& a, const std::vector<float>& b) const {
        if (a.size() != b.size()) return 0.0f;
        float dot = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) dot += a[i] * b[i];
        return dot;
    }

public:
    Matcher(const std::string& path) {
        llama_backend_init();

        llama_model_params mparams = llama_model_default_params();
        model = llama_model_load_from_file(path.c_str(), mparams);
        if (!model) throw std::runtime_error("Failed to load model");

        vocab = llama_model_get_vocab(model);
        
        if (!vocab) throw std::runtime_error("Failed to get vocab");

        // ****************  TEMP *******************
        if (!vocab) {
            std::cerr << "Vocab is null. Model may not be compatible.\n";
        } else {
            std::cout << "Vocab loaded successfully.\n";
        }
        // ****************  TEMP *******************
        
        if (!vocab) throw std::runtime_error("Failed to get vocab");

        llama_context_params cparams = llama_context_default_params();
        cparams.n_ctx = 512;
        cparams.embeddings = true;

        ctx = llama_init_from_model(model, cparams);
        if (!ctx) throw std::runtime_error("Context init failed");
    }

    ~Matcher() {
        if (ctx)   llama_free(ctx);
        if (model) llama_model_free(model);
        llama_backend_free();
    }

    std::vector<float> embed(const std::string &text) {
        std::vector<llama_token> tokens(512);
        
        int32_t n_tokens = llama_tokenize(
                                            vocab,                                // <-- first: vocab
                                            text.c_str(),                         // <-- second: text
                                            static_cast<int32_t>(text.size()),    // <-- third: text length
                                            tokens.data(),                        // <-- fourth: token buffer
                                            static_cast<int32_t>(tokens.size()),  // <-- fifth: buffer size
                                            true,                                 // <-- add_special (adds BOS)
                                            false                                 // <-- parse_special
        );
    
        if (n_tokens < 0) {
            std::cerr << "Tokenization failed or overflowed for: " << text << "\n";
            return {};
        }

        llama_batch batch = llama_batch_get_one(tokens.data(), n_tokens);
        if (llama_decode(ctx, batch) != 0) return {};

        const float *data = llama_get_embeddings(ctx);
        if (!data) return {};

        int dim = llama_model_n_embd(model);
        std::vector<float> vec(data, data + dim);

        float norm = 0.0f;
        for (float v : vec) norm += v*v;
        norm = std::sqrtf(norm);
        if (norm > 0.0f)
            for (auto &v : vec) v /= norm;

        return vec;
    }

    std::vector<QAPair> load(const std::string& csv_path) {
        std::vector<QAPair> db;
        std::ifstream f(csv_path);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            size_t p = line.find(',');
            if (p == std::string::npos) continue;

            std::string q = line.substr(0, p);
            std::string a = line.substr(p + 1);

            if (!q.empty() && q.front() == '"') { q.erase(0,1); q.pop_back(); }
            if (!a.empty() && a.front() == '"') { a.erase(0,1); a.pop_back(); }

            auto e = embed(q);
            if (!e.empty()) db.push_back({q, a, std::move(e)});
        }
        return db;
    }

    std::pair<std::string, float> find(const std::string& q,
                                       const std::vector<QAPair>& db) {
        auto qe = embed(q);
        if (qe.empty() || db.empty()) return {"No match", 0.0f};

        float best = -1.0f;
        std::string ans;
        for (const auto& p : db) {
            float s = cos_sim(qe, p.embedding);
            if (s > best) { best = s; ans = p.answer; }
        }
        return {ans, best};
    }
};


int main() {
    try {
        Matcher m("C:/dev/cpp/llm/models/tinyllama-q4.gguf");

        auto db = m.load("C:/dev/cpp/llm/matchQA/qa_database.csv");

        std::vector<std::string> queries = {
            "Can you tell me what is artificial intelligence is?",
            "Tell me about machine learning.",
            "How does a neural network work?",
            "Can you tell me what a bom means"
        };

        for (const auto& q : queries) {
            auto [a, s] = m.find(q, db);
            std::cout << "Q: " << q << "\n"
                      << "A: " << a << "\n"
                      << "Score: " << s << "\n\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}