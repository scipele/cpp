/*============================================================
fetch_intraday.cpp
Downloads Yahoo Finance intraday data.
Build:
g++ fetch_intraday.cpp -o fetch_intraday -lcurl -pthread -std=c++17 -O2
============================================================*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <mutex>
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <ctime>

namespace fs = std::filesystem;
using json = nlohmann::json;

constexpr char RANGE[]    = "20d";
constexpr char INTERVAL[] = "5m";

namespace fs = std::filesystem;

const fs::path OUTPUT_FOLDER = "./output";
const fs::path INPUT_FILE = "./input/tickers.csv";

std::mutex cout_mutex;


size_t WriteCallback(void* contents,
                     size_t size,
                     size_t nmemb,
                     void* userp)
{
    ((std::string*)userp)->append(
        (char*)contents,
        size * nmemb);

    return size * nmemb;
}

std::string fetch_url(const std::string& url,
                      int retries = 3)
{
    while (retries--)
    {
        CURL* curl = curl_easy_init();

        if (!curl)
            continue;

        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                         WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA,
                         &response);

        curl_easy_setopt(curl,
                         CURLOPT_USERAGENT,
                         "Mozilla/5.0");

        curl_easy_setopt(curl,
                         CURLOPT_TIMEOUT,
                         15L);

        curl_easy_setopt(curl,
                         CURLOPT_CONNECTTIMEOUT,
                         8L);

        CURLcode result =
            curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        if (result == CURLE_OK &&
            response.size() > 100)
        {
            return response;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(500));
    }

    return "";
}


std::string trim(std::string s)
{
    s.erase(
        0,
        s.find_first_not_of(" \t\r\n\""));

    s.erase(
        s.find_last_not_of(" \t\r\n\"") + 1);

    return s;
}


std::vector<std::string> load_tickers()
{
    std::vector<std::string> tickers;

    std::ifstream file(INPUT_FILE);

    if (!file)
    {
        std::cerr
            << "Cannot open "
            << INPUT_FILE
            << std::endl;

        return tickers;
    }

    std::string line;

    // Skip header
    std::getline(file, line);

    while (std::getline(file, line))
    {
        line = trim(line);

        if (!line.empty())
            tickers.push_back(line);
    }

    return tickers;
}


std::string make_url(const std::string& ticker)
{
    return
        "https://query1.finance.yahoo.com/v8/finance/chart/"
        + ticker +
        "?range=" + RANGE +
        "&interval=" + INTERVAL +
        "&includePrePost=false";
}


std::string format_timestamp(long timestamp)
{
    std::time_t t = timestamp;

    std::tm tm{};

    localtime_r(&t, &tm);

    char buffer[32];

    std::strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%d %H:%M",
        &tm);

    return buffer;
}


bool save_csv(const std::string& ticker,
              const std::string& raw_json)
{
    std::cout 
    << "Parsing "
    << ticker
    << "\n";

    try
    {
        json j = json::parse(raw_json);

        auto result =
            j["chart"]["result"][0];

        auto timestamps =
            result["timestamp"];

        auto quote =
            result["indicators"]["quote"][0];


        auto opens =
            quote["open"];

        auto highs =
            quote["high"];

        auto lows =
            quote["low"];

        auto closes =
            quote["close"];

        auto volumes =
            quote["volume"];


        fs::path filename =
            OUTPUT_FOLDER /
            (ticker + ".csv");


        std::ofstream out(filename);


        if (!out)
        {
            std::cerr
                << "Cannot create "
                << filename
                << "\n";

            return false;
        }


        out
            << "Ticker,Timestamp,Datetime,"
            << "Open,High,Low,Close,Volume\n";


        for (size_t i = 0;
             i < timestamps.size();
             i++)
        {

            // Yahoo sometimes returns null values
            if (opens[i].is_null() ||
                closes[i].is_null())
                continue;


            long ts =
                timestamps[i];


            out
                << ticker
                << ","
                << ts
                << ","
                << format_timestamp(ts)
                << ","
                << opens[i]
                << ","
                << highs[i]
                << ","
                << lows[i]
                << ","
                << closes[i]
                << ","
                << volumes[i]
                << "\n";
        }


        std::cout
            << "Saved "
            << filename
            << "\n";


        return true;

    }
    catch(const std::exception& e)
    {
        std::cerr
            << "JSON error for "
            << ticker
            << ": "
            << e.what()
            << "\n";

        return false;
    }
}


int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    auto tickers = load_tickers();

    std::cout
        << "Loaded "
        << tickers.size()
        << " tickers\n";

    for (const auto& ticker : tickers)
    {
        std::cout
            << "\nDownloading "
            << ticker
            << "...\n";


        std::string url =
            make_url(ticker);


        std::string raw =
            fetch_url(url);


        if (raw.empty())
        {
            std::cout
                << "FAILED "
                << ticker
                << "\n";

            continue;
        }


        std::cout
            << "Downloaded "
            << raw.size()
            << " bytes\n";

        save_csv(ticker,raw);
    }

    curl_global_cleanup();

    return 0;
}