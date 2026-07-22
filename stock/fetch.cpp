// fetch.cpp - Faster Version (Higher Concurrency)
// g++ fetch.cpp -o fetch -lcurl -std=c++17 -O3 -pthread

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <set>
#include <cmath>
#include <algorithm>

namespace fs = std::filesystem;
using json = nlohmann::json;

struct StockScore {
    std::string ticker;
    double short_score = 50.0;
    double long_score = 50.0;
    double rsi = 50.0;
    double momentum_3m = 0.0;
    double volatility = 0.0;
    double price = 0.0;
};

static std::mutex file_mutex;
static std::vector<StockScore> all_scores;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string fetch_url(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string response;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8L);           // faster timeout
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

std::set<std::string> extract_from_positions(const std::string& filename) {
    std::set<std::string> tickers;
    std::ifstream file(filename);
    if (!file.is_open()) return tickers;
    std::string line;
    std::getline(file, line);
    std::getline(file, line);
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, ',');
        if (!token.empty() && token.front() == '"') token.erase(0, 1);
        if (!token.empty() && token.back() == '"') token.pop_back();
        if (!token.empty() && token != "Symbol" && token.length() <= 10) {
            tickers.insert(token);
        }
    }
    return tickers;
}

std::vector<std::string> load_all_tickers() {
    std::set<std::string> unique_tickers;
    std::ifstream tickers_file("input/tickers.csv");
    if (tickers_file.is_open()) {
        std::string line;
        std::getline(tickers_file, line);
        while (std::getline(tickers_file, line)) {
            if (line.empty()) continue;
            line.erase(0, line.find_first_not_of(" \t\""));
            line.erase(line.find_last_not_of(" \t\"") + 1);
            if (!line.empty()) unique_tickers.insert(line);
        }
    }
    for (const auto& entry : fs::directory_iterator("input")) {
        std::string path = entry.path().string();
        if (path.find("Positions") != std::string::npos && path.size() > 4 && path.substr(path.size()-4) == ".csv") {
            std::cout << "📄 Found: " << path << std::endl;
            auto pos = extract_from_positions(path);
            unique_tickers.insert(pos.begin(), pos.end());
        }
    }
    return std::vector<std::string>(unique_tickers.begin(), unique_tickers.end());
}

StockScore compute_scores(const json& chart_data, const std::string& ticker) {
    StockScore score;
    score.ticker = ticker;
    try {
        auto closes_json = chart_data["indicators"]["quote"][0]["close"];
        int n = closes_json.size();
        if (n < 30) return score;

        std::vector<double> closes(n);
        for (int i = 0; i < n; ++i) closes[i] = closes_json[i].get<double>();

        score.price = closes.back();

        double gain = 0.0, loss = 0.0;
        for (int i = n - 14; i < n; ++i) {
            double change = closes[i] - closes[i-1];
            if (change > 0) gain += change;
            else loss -= change;
        }
        double avg_gain = gain / 14.0;
        double avg_loss = loss / 14.0;
        score.rsi = (avg_loss == 0) ? 100.0 : 100.0 - (100.0 / (1.0 + avg_gain / avg_loss));

        if (n > 63) {
            score.momentum_3m = (closes.back() / closes[n - 64] - 1.0) * 100.0;
        }

        int bb = 20;
        double sum = 0.0;
        for (int i = n - bb; i < n; ++i) sum += closes[i];
        double sma = sum / bb;
        double var = 0.0;
        for (int i = n - bb; i < n; ++i) {
            double d = closes[i] - sma;
            var += d * d;
        }
        double stddev = std::sqrt(var / bb);
        score.volatility = ((sma + 2*stddev) - (sma - 2*stddev)) / sma * 100.0;

        score.short_score = 35 + (score.rsi - 50) * 1.1 + score.momentum_3m * 0.75 + (score.volatility - 15) * 0.6;
        score.long_score = 35 + score.momentum_3m * 0.55 + (70 - std::abs(score.rsi - 55)) * 0.8 + (25 - score.volatility) * 1.1;

        std::hash<std::string> hasher;
        double jitter = (hasher(ticker) % 800) / 160.0 - 2.5;
        score.short_score += jitter;
        score.long_score += jitter * 0.6;

        score.short_score = std::max(8.0, std::min(97.0, score.short_score));
        score.long_score = std::max(8.0, std::min(97.0, score.long_score));

    } catch (...) {}
    return score;
}

void fetch_and_save(const std::string& ticker) {
    std::string url = "https://query1.finance.yahoo.com/v8/finance/chart/" + ticker + "?range=1y&interval=1d";
    std::string raw = fetch_url(url);
    if (raw.length() < 1000) return;

    try {
        json j = json::parse(raw);
        json chart = j["chart"]["result"][0];
        StockScore s = compute_scores(chart, ticker);

        {
            std::lock_guard<std::mutex> lock(file_mutex);
            all_scores.push_back(s);
        }

        std::lock_guard<std::mutex> lock(file_mutex);
        std::ofstream out("output/bulk_stock_data.jsonl", std::ios::app);
        out << "{\"ticker\":\"" << ticker << "\",\"price\":" << s.price
            << ",\"rsi\":" << s.rsi << ",\"mom_3m\":" << s.momentum_3m
            << ",\"volatility\":" << s.volatility
            << ",\"short_score\":" << s.short_score 
            << ",\"long_score\":" << s.long_score 
            << ",\"ts\":" << std::time(nullptr) << "}\n";

        std::cout << "✅ " << ticker 
                  << "  S:" << (int)s.short_score 
                  << "  L:" << (int)s.long_score << std::endl;
    } catch (...) {}
}

void generate_summary() { /* keep the same as last version */ 
    if (all_scores.empty()) return;

    std::ofstream summary("output/summary_all.csv");

    auto write_row = [&](const StockScore& s, int rank, double combined) {
        summary << rank << "," << s.ticker << "," 
                << s.short_score << "," << s.long_score << "," << combined << ","
                << s.rsi << "," << s.momentum_3m << "," << s.volatility 
                << "," << s.price << "\n";
    };

    std::sort(all_scores.begin(), all_scores.end(), [](const StockScore& a, const StockScore& b){
        return a.short_score > b.short_score;
    });
    summary << "=== TOP 20 SHORT TERM ===\n";
    summary << "Rank,Ticker,Short_Score,Long_Score,Combined_Score,RSI,Momentum,Volatility,Price\n";
    for (size_t i = 0; i < all_scores.size() && i < 20; ++i) {
        double combined = (all_scores[i].short_score + all_scores[i].long_score) / 2.0;
        write_row(all_scores[i], i+1, combined);
    }

    summary << "\n=== TOP 20 LONG TERM ===\n";
    std::sort(all_scores.begin(), all_scores.end(), [](const StockScore& a, const StockScore& b){
        return a.long_score > b.long_score;
    });
    summary << "Rank,Ticker,Short_Score,Long_Score,Combined_Score,RSI,Momentum,Volatility,Price\n";
    for (size_t i = 0; i < all_scores.size() && i < 20; ++i) {
        double combined = (all_scores[i].short_score + all_scores[i].long_score) / 2.0;
        write_row(all_scores[i], i+1, combined);
    }

    summary << "\n=== TOP 20 COMBINED ===\n";
    std::sort(all_scores.begin(), all_scores.end(), [](const StockScore& a, const StockScore& b){
        return (a.short_score + a.long_score) > (b.short_score + b.long_score);
    });
    summary << "Rank,Ticker,Short_Score,Long_Score,Combined_Score,RSI,Momentum,Volatility,Price\n";
    for (size_t i = 0; i < all_scores.size() && i < 20; ++i) {
        double combined = (all_scores[i].short_score + all_scores[i].long_score) / 2.0;
        write_row(all_scores[i], i+1, combined);
    }

    std::cout << "📊 summary_all.csv generated!\n";
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    std::cout << "🚀 Fast Fetcher (15 threads)\n";
    
    auto tickers = load_all_tickers();
    std::cout << "Fetching " << tickers.size() << " tickers...\n\n";

    std::ofstream("output/bulk_stock_data.jsonl", std::ios::trunc);

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    const int max_threads = 15;   // Increased

    for (const auto& t : tickers) {
        threads.emplace_back(fetch_and_save, t);
        if (threads.size() >= max_threads) {
            for (auto& th : threads) th.join();
            threads.clear();
        }
    }
    for (auto& th : threads) th.join();

    generate_summary();

    auto duration = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now() - start).count();

    std::cout << "\n🎉 Finished in " << duration << " seconds.\n";

    curl_global_cleanup();
    return 0;
}