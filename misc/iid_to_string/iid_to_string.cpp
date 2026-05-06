// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | iid_to_string.cpp                                           |
//| EntryPoint   | main                                                        |
//| Purpose      | test speed conversion of IIDs to strings                    |
//| Inputs       | hard coded hex array values                                 |
//| Outputs      | print results                                               |
//| Dependencies | std libs                                                    |
//| By Name,Date | T.Sciple, 5/5/2026                                          |


#include <array>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>

using IID = unsigned char;

void IIDToStringFast2(const unsigned char* iid, char* out);
void IIDToStringFast3(const unsigned char* iid, char* out);
void IIDToStringFast4(const unsigned char* iid, char* out);

static const unsigned char order[16] = {
    3,2,1,0,  5,4,  7,6,  8,9,10,11,12,13,14,15
};

static const char hex[] = "0123456789abcdef";


int main() {
    using IID = std::array<unsigned char, 16>;

    // Sample data
    const std::array<IID, 8> testData = {{
        {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
        {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}},
        {{0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE}},
        {{0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0xEF, 0xCD, 0xAB, 0x89, 0x6A, 0x45, 0x23, 0x01}},
        {{0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA}},
        {{0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55}},
        {{0x0F, 0xF0, 0x1E, 0xE1, 0x2D, 0xD2, 0x3C, 0xC3, 0x4B, 0xB4, 0x5A, 0xA5, 0x69, 0x96, 0x78, 0x87}},
        {{0x13, 0x37, 0xC0, 0xDE, 0xBE, 0xEF, 0xFA, 0xCE, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0}}
    }};

    constexpr std::uint64_t loops = 40'000'000;

    auto runBench = [&](const char* name, void (*fn)(const unsigned char*, char*)) {
        // char array with 32 hex chars + 4 dashes + null terminator
        char out[37] = {};  
        volatile std::uint64_t sink = 0;

        const auto t0 = std::chrono::steady_clock::now();

        for (std::uint64_t i = 0; i < loops; ++i) {
            const IID& iid = testData[i % testData.size()];
            fn(iid.data(), out);
            sink += static_cast<unsigned char>(out[0]);
        }

        const auto t1 = std::chrono::steady_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        const double sec = static_cast<double>(ms) / 1000.0;
        double rate = loops / sec;
        double pct_faster = 0.0;
        

        std::cout   << name << '\n'
                    << "  Sample output: " << out << '\n'
                    << "  Iterations: " << loops << '\n'
                    << "  Elapsed: " << ms << " ms\n"
                    << "  Rate: " << std::fixed << std::setprecision(2)
                    << rate << " conversions/sec\n"
                    << "  Sink: " << sink << "\n";
        static double baseline_rate = 0.0;
        if (name == "IIDToStringFast2") {
            baseline_rate = rate;
            std::cout << "  This is the baseline for comparison.\n\n";
        } else {
            pct_faster = ((rate - baseline_rate) / baseline_rate) * 100.0;
            std::cout << "  Percent faster than IIDToStringFast2: " << pct_faster << "%\n\n";
        }
    };

    runBench("IIDToStringFast2", IIDToStringFast2);
    runBench("IIDToStringFast3", IIDToStringFast3);
    runBench("IIDToStringFast4", IIDToStringFast4);

    return 0;
}


void IIDToStringFast2(const IID *iid, char* out) {
    
    const unsigned char *p = (const unsigned char *)iid;
    int i,j = 0;
    
    for (int i = 0; i < 16; i++) {
        unsigned char b = p[order[i]];

        out[j++] = hex[b >> 4];
        out[j++] = hex[b & 0xF];

        if (j == 8 || j == 13 || j == 18 || j == 23) {
            out[j++] = '-';
        }
    }

    out[j] = '\0';
}


void IIDToStringFast3(const IID *iid, char* out) {
    const unsigned char *p = (const unsigned char *)iid;
    int i,j = 0;
    unsigned char b;
    for (int i = 3; i >= 0; i--) {
        b = p[i];
        out[j++] = hex[b >> 4]; out[j++] = hex[b & 0x0F];  // left shifts high nibble, masks low nibble with 00001111 (0x0F)
    }
    out[j++] = '-';
    for (int i = 5; i > 3; i--) {
        b = p[i];
        out[j++] = hex[b >> 4]; out[j++] = hex[b & 0x0F];
    }
    out[j++] = '-';
    for (int i = 7; i > 5; i--) {
        b = p[i];
        out[j++] = hex[b >> 4]; out[j++] = hex[b & 0x0F];
    }
    out[j++] = '-';
    for (int i = 8; i < 16; i++) {
        b = p[i];
        out[j++] = hex[b >> 4]; out[j++] = hex[b & 0x0F];
    }
    out[j] = '\0';
}


void IIDToStringFast4(const IID *iid, char* out) {
    const unsigned char *p = (const unsigned char *)iid;
    unsigned char b;

    b = p[3];  *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    b = p[2];  *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    b = p[1];  *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    b = p[0];  *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    *out++ = '-';

    b = p[5];  *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    b = p[4];  *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    *out++ = '-';

    b = p[7];  *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    b = p[6];  *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    *out++ = '-';

    b = p[8];  *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    b = p[9];  *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    b = p[10]; *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    b = p[11]; *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    b = p[12]; *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    b = p[13]; *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    b = p[14]; *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];
    b = p[15]; *out++ = hex[b >> 4]; *out++ = hex[b & 0xF];

    *out = '\0';
}


