//filename: FileHasher.hpp
#pragma once

#include <array>
#include <codecvt>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>


const std::uintmax_t MAX_FILE_SIZE_FOR_HASHING = 50 * 1024 * 1024; // 50 MB

class FileHasher {

public:

    static std::wstring sha1FileHash(const std::filesystem::path& filePath) {
        try {
            if (std::filesystem::file_size(filePath) > MAX_FILE_SIZE_FOR_HASHING) {
                return L"Skipped Hash greater than 50 mb";
            }
    
            // SHA-1 initialization constants
            uint32_t h[5] = {
                0x67452301,
                0xEFCDAB89,
                0x98BADCFE,
                0x10325476,
                0xC3D2E1F0
            };

            std::ifstream file(filePath, std::ios::binary);
            if (!file) {
                return L"Error in Hash";
            }

            std::array<uint8_t, 65536> buffer;
            uint64_t totalBytesRead = 0;

            while (file) {
                file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
                std::streamsize bytesRead = file.gcount();
                totalBytesRead += bytesRead;

                if (bytesRead == 64) {
                    process_chunk(buffer.data(), h);
                } else {
                    // Handle padding for the final chunk
                    std::vector<uint8_t> finalChunk(buffer.begin(), buffer.begin() + bytesRead);
                    finalChunk.push_back(0x80);

                    if (bytesRead < 56) {
                        finalChunk.resize(56, 0);
                    } else {
                        finalChunk.resize(64, 0);
                        process_chunk(finalChunk.data(), h);
                        finalChunk.assign(56, 0);
                    }

                    uint64_t totalBits = totalBytesRead * 8;
                    for (int i = 0; i < 8; ++i) {
                        finalChunk.push_back((totalBits >> (56 - 8 * i)) & 0xFF);
                    }

                    process_chunk(finalChunk.data(), h);
                }
            }

            // Convert hash to hexadecimal string
            std::stringstream hash_ss;
            hash_ss << std::hex << std::setfill('0');
            for (uint32_t i : h) {
                hash_ss << std::setw(8) << i;
            }

            return string_to_wstring(hash_ss.str());
        } catch (const std::exception&) {
            return L"Error in Hash";
        }
    }

private:

    // Helper functions for SHA-1
    static uint32_t left_rotate(uint32_t value, unsigned int count) {
        return (value << count) | (value >> (32 - count));
    }

    static void process_chunk(const uint8_t* chunk, uint32_t* h) {
        uint32_t w[80];
        for (int i = 0; i < 16; ++i) {
            w[i] = (chunk[i * 4] << 24) | (chunk[i * 4 + 1] << 16) | (chunk[i * 4 + 2] << 8) | (chunk[i * 4 + 3]);
        }
        for (int i = 16; i < 80; ++i) {
            w[i] = left_rotate(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }

        uint32_t a = h[0];
        uint32_t b = h[1];
        uint32_t c = h[2];
        uint32_t d = h[3];
        uint32_t e = h[4];

        for (int i = 0; i < 80; ++i) {
            uint32_t f, k;
            if (i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            uint32_t temp = left_rotate(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = left_rotate(b, 30);
            b = a;
            a = temp;
        }

        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
        h[4] += e;
    }

    static std::wstring string_to_wstring(const std::string &str) {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(str);
    }
};