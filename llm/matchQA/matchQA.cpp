#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include "../llama.cpp/include/llama.h"

struct QAPair {
    std::string question;
    std::string answer;
    std::vector<float> embedding;
};

class Matcher {
    llama_model* model = nullptr;
    const llama_vocab* vocab = nullptr;
    llama_context* ctx = nullptr;

    // Cosine similarity
    float cos_sim(const std::vector<float>& a, const std::vector<float>& b) const {
        if (a.size() != b.size() || a.empty()) return 0.0f;
        float dot = 0.0f, na = 0.0f, nb = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            dot += a[i] * b[i];
            na  += a[i] * a[i];
            nb  += b[i] * b[i];
        }
        return dot / (std::sqrt(na) * std::sqrt(nb) + 1e-9f);
    }

public:
    Matcher(const std::string& path) {
        llama_backend_init();

        llama_model_params mparams = llama_model_default_params();
        model = llama_load_model_from_file(path.c_str(), mparams);
        
        if (!model) throw std::runtime_error("Failed to load model");

        vocab = llama_model_get_vocab(model);
        if (!vocab) throw std::runtime_error("Failed to get vocab");

        llama_context_params cparams = llama_context_default_params();
        cparams.n_ctx = 512;
        cparams.embeddings = true;  // enable embedding mode

        ctx = llama_init_from_model(model, cparams);
        if (!ctx) throw std::runtime_error("Failed to initialize context");

        std::cout << "Model loaded. Embedding dimension: " << llama_model_n_embd(model) << "\n";
    }

    ~Matcher() {
        if (ctx) llama_free(ctx);
        if (model) llama_model_free(model);
        llama_backend_free();
    }

    std::vector<float> embed(const std::string &text) {
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

        if (n_tokens <= 0) {
            std::cerr << "Tokenization failed for: " << full_text << "\n";
            return {};
        }

        // Evaluate tokens (mandatory for GGUF embeddings)
        if (llama_eval(ctx, tokens.data(), n_tokens, 0, 1) != 0) {
            std::cerr << "llama_eval failed for: " << full_text << "\n";
            return {};
        }

        // Get embeddings
        const float* data = llama_get_embeddings(ctx);
        if (!data) {
            std::cerr << "No embeddings returned for: " << full_text << "\n";
            return {};
        }

        int dim = llama_model_n_embd(model);

        std::vector<float> result(data, data + dim);

        // Optional: print first 10 values for debugging
        std::cout << "First 10 embedding values: ";
        for (int i = 0; i < 10; ++i) std::cout << result[i] << " ";
        std::cout << "\n";
        return result;
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
        Matcher m("C:/dev/cpp/llm/models/bge-small-en-v1.5-f16.gguf");

        auto db = m.load("C:/dev/cpp/llm/matchQA/qa_database.csv");
        
        std::vector<std::string> queries = {
            "Can you tell me what is artificial intelligence is?",
            "Tell me about machine learning.",
            "How does a neural network work?",
            "Can you tell me more about what a bom is?"
        };

        std::cout << "\nDatabase Questions and Answers Read in:\n";

        // Confirm the qa items are read correctly

        for (auto& item : db) {
            std::cout << "question: " << item.question
                      << "\nanswer: " << item.answer << "\n\n";
        }

        std::cout << "\n\n";
        std::cout << "Questions and Closest Matches\n";

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