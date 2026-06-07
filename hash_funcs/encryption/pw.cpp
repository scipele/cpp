// Compile on Windows/ucrt64 with:
//c:/msys64/ucrt64/bin/g++.exe -std=c++17 -g c:/dev/cpp/hash_funcs/encryption/pw.cpp -o c:/dev/cpp/hash_funcs/encryption/pw.exe -lssl -lcrypto

// Compile on Linux with:
// g++ -fdiagnostics-color=always -g /home/dev/cpp/hash_funcs/encryption/pw.cpp -o /home/dev/cpp/hash_funcs/encryption/pw -lssl -lcrypto


#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <chrono>
#include <ctime>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>

// Print errors from the OpenSSL queue if a cryptographic step fails
void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

// Encrypts plaintext using AES-256-CBC
std::vector<unsigned char> aes_256_cbc_encrypt(const std::string& plaintext,
                                               const std::vector<unsigned char>& key,
                                               const std::vector<unsigned char>& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    // Initialize encryption operation with AES-256-CBC
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data())) {
        handleErrors();
    }

    // Allocate buffer for ciphertext (include space for block padding)
    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0;
    int ciphertext_len = 0;

    // Provide the message to be encrypted
    if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                               reinterpret_cast<const unsigned char*>(plaintext.data()),
                               plaintext.size())) {
        handleErrors();
    }
    ciphertext_len = len;

    // Finalize the encryption (handles remaining PKCS#7 padding)
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len)) {
        handleErrors();
    }
    ciphertext_len += len;

    // Clean up context and resize container to exact size
    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

// Decrypts ciphertext using AES-256-CBC
std::string aes_256_cbc_decrypt(const std::vector<unsigned char>& ciphertext,
                                const std::vector<unsigned char>& key,
                                const std::vector<unsigned char>& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    // Initialize decryption operation
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data())) {
        handleErrors();
    }

    // Allocate buffer for plaintext
    std::vector<unsigned char> plaintext_buf(ciphertext.size());
    int len = 0;
    int plaintext_len = 0;

    // Provide the bytes to be decrypted
    if (1 != EVP_DecryptUpdate(ctx, plaintext_buf.data(), &len, ciphertext.data(), ciphertext.size())) {
        handleErrors();
    }
    plaintext_len = len;

    // Finalize decryption and strip padding.
    // Failure here usually implies an incorrect key or corrupted data.
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext_buf.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return std::string(plaintext_buf.begin(), plaintext_buf.begin() + plaintext_len);
}

struct StoredCredential {
    std::vector<unsigned char> iv;
    std::vector<unsigned char> ciphertext;
};

std::string bytes_to_hex(const std::vector<unsigned char>& bytes) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (unsigned char b : bytes) {
        oss << std::setw(2) << static_cast<int>(b);
    }
    return oss.str();
}

std::vector<unsigned char> hex_to_bytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    if (hex.size() % 2 != 0) return bytes;

    bytes.reserve(hex.size() / 2);
    for (std::size_t i = 0; i < hex.size(); i += 2) {
        const std::string byte_str = hex.substr(i, 2);
        unsigned int value = 0;
        std::istringstream iss(byte_str);
        iss >> std::hex >> value;
        if (iss.fail()) {
            return {};
        }
        bytes.push_back(static_cast<unsigned char>(value));
    }
    return bytes;
}

bool contains_pipe(const std::string& value) {
    return value.find('|') != std::string::npos;
}

std::vector<unsigned char> compute_auth_tag(const std::vector<unsigned char>& key) {
    static constexpr char kVaultAuthMessage[] = "vault-auth-check";
    unsigned int tag_len = 0;
    std::vector<unsigned char> tag(EVP_MAX_MD_SIZE);
    unsigned char* result = HMAC(EVP_sha256(),
                                 key.data(),
                                 static_cast<int>(key.size()),
                                 reinterpret_cast<const unsigned char*>(kVaultAuthMessage),
                                 sizeof(kVaultAuthMessage) - 1,
                                 tag.data(),
                                 &tag_len);
    if (!result) {
        handleErrors();
    }
    tag.resize(tag_len);
    return tag;
}

bool validate_master_password(const std::vector<unsigned char>& key,
                              const std::vector<unsigned char>& expected_auth_tag) {
    return compute_auth_tag(key) == expected_auth_tag;
}

std::vector<unsigned char> derive_key_from_password(const std::string& master_password,
                                                    const std::vector<unsigned char>& salt) {
    std::vector<unsigned char> key(32);
    const int iterations = 200000;
    if (1 != PKCS5_PBKDF2_HMAC(master_password.c_str(),
                               static_cast<int>(master_password.size()),
                               salt.data(),
                               static_cast<int>(salt.size()),
                               iterations,
                               EVP_sha256(),
                               static_cast<int>(key.size()),
                               key.data())) {
        handleErrors();
    }
    return key;
}

StoredCredential encrypt_credential_record(const std::string& plain_record,
                                           const std::vector<unsigned char>& key) {
    std::vector<unsigned char> iv(16);
    if (1 != RAND_bytes(iv.data(), static_cast<int>(iv.size()))) {
        handleErrors();
    }

    StoredCredential record;
    record.iv = iv;
    record.ciphertext = aes_256_cbc_encrypt(plain_record, key, iv);
    return record;
}

std::string decrypt_credential_record(const StoredCredential& record,
                                      const std::vector<unsigned char>& key) {
    return aes_256_cbc_decrypt(record.ciphertext, key, record.iv);
}

std::string build_plain_record(const std::string& resource_name,
                               const std::string& description,
                               const std::string& user_name,
                               const std::string& password,
                               const std::string& password_last_modified) {
    return resource_name + "|" + description + "|" + user_name + "|" + password + "|" +
           password_last_modified + "|";
}

bool parse_plain_record(const std::string& plain_record,
                        std::string& resource_name,
                        std::string& description,
                        std::string& user_name,
                        std::string& password,
                        std::string& password_last_modified) {
    std::vector<std::string> fields;
    std::string current;

    for (char ch : plain_record) {
        if (ch == '|') {
            fields.push_back(current);
            current.clear();
            continue;
        }
        current.push_back(ch);
    }

    if (fields.size() < 4) {
        return false;
    }

    resource_name = fields[0];
    description = fields[1];
    user_name = fields[2];
    password = fields[3];
    password_last_modified = fields.size() >= 5 ? fields[4] : "";
    return true;
}

std::string current_local_timestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::tm local_tm{};
#ifdef _WIN32
    localtime_s(&local_tm, &now_time);
#else
    localtime_r(&now_time, &local_tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

struct DisplayRecord {
    std::size_t storage_index;
    std::string plain;
    std::string sort_key;
};

std::string to_lower_ascii(std::string value) {
    for (char& ch : value) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return value;
}

std::string first_field(const std::string& pipe_record) {
    const std::size_t p = pipe_record.find('|');
    if (p == std::string::npos) {
        return pipe_record;
    }
    return pipe_record.substr(0, p);
}

std::vector<DisplayRecord> build_sorted_decrypted_records(const std::vector<StoredCredential>& records,
                                                          const std::vector<unsigned char>& key,
                                                          std::size_t& skipped) {
    std::vector<DisplayRecord> output;
    skipped = 0;

    output.reserve(records.size());
    for (std::size_t i = 0; i < records.size(); ++i) {
        const std::string plain = decrypt_credential_record(records[i], key);
        if (plain.empty()) {
            ++skipped;
            continue;
        }

        output.push_back(DisplayRecord{i, plain, to_lower_ascii(first_field(plain))});
    }

    std::sort(output.begin(), output.end(), [](const DisplayRecord& a, const DisplayRecord& b) {
        if (a.sort_key == b.sort_key) {
            return a.plain < b.plain;
        }
        return a.sort_key < b.sort_key;
    });

    return output;
}

std::vector<unsigned char> load_or_create_salt(const std::string& file_path) {
    std::ifstream in(file_path);
    if (in) {
        std::string first_line;
        if (std::getline(in, first_line)) {
            const std::string prefix = "SALT|";
            if (first_line.rfind(prefix, 0) == 0) {
                std::vector<unsigned char> salt = hex_to_bytes(first_line.substr(prefix.size()));
                if (salt.size() == 16) {
                    return salt;
                }
            }
        }
    }

    std::vector<unsigned char> salt(16);
    if (1 != RAND_bytes(salt.data(), static_cast<int>(salt.size()))) {
        handleErrors();
    }
    return salt;
}

void load_records_skip_header(const std::string& file_path, std::vector<StoredCredential>& records) {
    std::ifstream in(file_path);
    if (!in) {
        return;
    }

    std::string line;
    bool first_line_checked = false;
    while (std::getline(in, line)) {
        if (!first_line_checked) {
            first_line_checked = true;
            if (line.rfind("SALT|", 0) == 0) {
                continue;
            }
        }

        if (line.rfind("AUTH|", 0) == 0) {
            continue;
        }

        if (line.empty()) continue;

        const std::size_t delim = line.find('|');
        if (delim == std::string::npos) continue;

        const std::string iv_hex = line.substr(0, delim);
        const std::string ct_hex = line.substr(delim + 1);

        StoredCredential record;
        record.iv = hex_to_bytes(iv_hex);
        record.ciphertext = hex_to_bytes(ct_hex);

        if (record.iv.size() == 16 && !record.ciphertext.empty()) {
            records.push_back(record);
        }
    }
}

struct VaultMetadata {
    std::vector<unsigned char> salt;
    std::vector<unsigned char> auth_tag;
    bool has_auth_tag = false;
};

VaultMetadata load_vault_metadata(const std::string& file_path) {
    VaultMetadata metadata;

    std::ifstream in(file_path);
    if (!in) {
        return metadata;
    }

    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("SALT|", 0) == 0) {
            metadata.salt = hex_to_bytes(line.substr(5));
        } else if (line.rfind("AUTH|", 0) == 0) {
            metadata.auth_tag = hex_to_bytes(line.substr(5));
            metadata.has_auth_tag = !metadata.auth_tag.empty();
        } else {
            break;
        }
    }

    return metadata;
}

bool save_records(const std::string& file_path,
                  const std::vector<unsigned char>& salt,
                  const std::vector<unsigned char>& auth_tag,
                  const std::vector<StoredCredential>& records) {
    std::ofstream out(file_path, std::ios::trunc);
    if (!out) return false;

    out << "SALT|" << bytes_to_hex(salt) << "\n";
    out << "AUTH|" << bytes_to_hex(auth_tag) << "\n";

    for (const auto& record : records) {
        out << bytes_to_hex(record.iv) << "|" << bytes_to_hex(record.ciphertext) << "\n";
    }
    return true;
}

void print_decrypted_records(const std::vector<DisplayRecord>& records,
                             std::size_t skipped) {
    std::cout << "idx|resource_name|description|user_name|password|password_last_modified|\n";
    for (std::size_t i = 0; i < records.size(); ++i) {
        std::string resource_name;
        std::string description;
        std::string user_name;
        std::string password;
        std::string password_last_modified;

        if (parse_plain_record(records[i].plain,
                               resource_name,
                               description,
                               user_name,
                               password,
                               password_last_modified)) {
            std::cout << (i + 1) << "|"
                      << resource_name << "|"
                      << description << "|"
                      << user_name << "|"
                      << password << "|"
                      << password_last_modified << "|\n";
        } else {
            // Fallback for unexpected record formats.
            std::cout << (i + 1) << "|" << records[i].plain << "|\n";
        }
    }
    if (skipped > 0) {
        std::cout << "# skipped_unreadable_records=" << skipped << "\n";
    }
}

int read_menu_choice() {
    std::string line;
    std::getline(std::cin, line);
    if (line.empty()) return -1;

    try {
        return std::stoi(line);
    } catch (...) {
        return -1;
    }
}

std::string read_hidden_input(const std::string& prompt) {
    std::cout << prompt;
    std::cout.flush();

#ifdef _WIN32
    std::string value;
    while (true) {
        const int ch = _getch();
        if (ch == '\r' || ch == '\n') {
            std::cout << "\n";
            break;
        }
        if (ch == '\b') {
            if (!value.empty()) {
                value.pop_back();
            }
            continue;
        }
        if (ch >= 32 && ch <= 126) {
            value.push_back(static_cast<char>(ch));
        }
    }
    return value;
#else
    termios oldt{};
    if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
        std::string fallback;
        std::getline(std::cin, fallback);
        return fallback;
    }

    termios newt = oldt;
    newt.c_lflag &= static_cast<unsigned int>(~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    std::string value;
    std::getline(std::cin, value);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << "\n";
    return value;
#endif
}

int main(int argc, char* argv[]) {
    std::string vault_file = "pw.vault";
    if (const char* env_path = std::getenv("VAULT_FILE")) {
        if (env_path[0] != '\0') {
            vault_file = env_path;
        }
    }
    if (argc > 1 && argv[1] != nullptr && argv[1][0] != '\0') {
        vault_file = argv[1];
    }

    VaultMetadata metadata = load_vault_metadata(vault_file);
    if (metadata.salt.empty()) {
        metadata.salt = load_or_create_salt(vault_file);
    }

    std::vector<unsigned char> key;
    bool unlocked = false;
    for (int attempt = 1; attempt <= 5; ++attempt) {
        std::string master_password = read_hidden_input("Enter master password: ");
        if (master_password.empty()) {
            std::cout << "Master password cannot be empty.\n";
            continue;
        }

        key = derive_key_from_password(master_password, metadata.salt);
        std::fill(master_password.begin(), master_password.end(), '\0');

        if (!metadata.has_auth_tag) {
            metadata.auth_tag = compute_auth_tag(key);
            metadata.has_auth_tag = true;
            unlocked = true;
            break;
        }

        if (validate_master_password(key, metadata.auth_tag)) {
            unlocked = true;
            break;
        }

        std::cout << "Incorrect master password. Attempts remaining: " << (5 - attempt) << "\n";
    }

    if (!unlocked) {
        std::cerr << "Too many failed attempts. Exiting.\n";
        return 1;
    }

    std::vector<StoredCredential> records;
    load_records_skip_header(vault_file, records);

    std::cout << "Encrypted credential manager (AES-256-CBC)\n";
    std::cout << "Active vault: " << vault_file << "\n";
    std::cout << "Loaded records: " << records.size() << "\n";

    while (true) {
        std::cout << "\nChoose an option:\n";
        std::cout << "1) Add credential\n";
        std::cout << "2) Show decrypted records (pipe-delimited CSV format)\n";
        std::cout << "3) Edit record by index\n";
        std::cout << "4) Delete record by index\n";
        std::cout << "5) Save\n";
        std::cout << "6) Exit without saving\n";
        std::cout << "7) Save and exit\n";
        std::cout << "> ";

        const int choice = read_menu_choice();
        switch (choice) {
            case 1: {
                std::string resource_name;
                std::string description;
                std::string user_name;
                std::string password;

                std::cout << "resource_name (site/app/device): ";
                std::getline(std::cin, resource_name);
                std::cout << "description: ";
                std::getline(std::cin, description);
                std::cout << "user_name: ";
                std::getline(std::cin, user_name);
                std::cout << "password: ";
                std::getline(std::cin, password);

                if (resource_name.empty() || description.empty() || user_name.empty() || password.empty()) {
                    std::cout << "All fields are required.\n";
                    break;
                }
                if (contains_pipe(resource_name) || contains_pipe(description) || contains_pipe(user_name) || contains_pipe(password)) {
                    std::cout << "Values cannot contain '|'.\n";
                    break;
                }

                const std::string password_last_modified = current_local_timestamp();
                const std::string plain_record = build_plain_record(resource_name,
                                                                    description,
                                                                    user_name,
                                                                    password,
                                                                    password_last_modified);
                records.push_back(encrypt_credential_record(plain_record, key));
                std::cout << "Credential added (encrypted in memory).\n";
                break;
            }
            case 2: {
                std::size_t skipped = 0;
                const std::vector<DisplayRecord> display = build_sorted_decrypted_records(records, key, skipped);
                print_decrypted_records(display, skipped);
                break;
            }
            case 3: {
                if (records.empty()) {
                    std::cout << "No records to edit.\n";
                    break;
                }

                std::size_t skipped = 0;
                const std::vector<DisplayRecord> display = build_sorted_decrypted_records(records, key, skipped);
                if (display.empty()) {
                    std::cout << "No readable records to edit with current master password.\n";
                    break;
                }

                print_decrypted_records(display, skipped);
                std::cout << "Enter index to edit (1-" << display.size() << "): ";
                const int idx = read_menu_choice();
                if (idx < 1 || static_cast<std::size_t>(idx) > display.size()) {
                    std::cout << "Invalid index.\n";
                    break;
                }

                const DisplayRecord& selected = display[idx - 1];
                std::string resource_name;
                std::string description;
                std::string user_name;
                std::string password;
                std::string password_last_modified;
                if (!parse_plain_record(selected.plain,
                                        resource_name,
                                        description,
                                        user_name,
                                        password,
                                        password_last_modified)) {
                    std::cout << "Unable to parse selected record.\n";
                    break;
                }

                std::string updated_value;
                std::cout << "resource_name (current: " << resource_name << "): ";
                std::getline(std::cin, updated_value);
                if (!updated_value.empty()) {
                    resource_name = updated_value;
                }

                std::cout << "description (current: " << description << "): ";
                std::getline(std::cin, updated_value);
                if (!updated_value.empty()) {
                    description = updated_value;
                }

                std::cout << "user_name (current: " << user_name << "): ";
                std::getline(std::cin, updated_value);
                if (!updated_value.empty()) {
                    user_name = updated_value;
                }

                std::cout << "password (leave blank to keep current): ";
                std::getline(std::cin, updated_value);
                bool password_changed = false;
                if (!updated_value.empty()) {
                    password_changed = (updated_value != password);
                    password = updated_value;
                }

                if (password_changed) {
                    password_last_modified = current_local_timestamp();
                }

                if (resource_name.empty() || description.empty() || user_name.empty() || password.empty()) {
                    std::cout << "All fields are required.\n";
                    break;
                }
                if (contains_pipe(resource_name) || contains_pipe(description) || contains_pipe(user_name) ||
                    contains_pipe(password) || contains_pipe(password_last_modified)) {
                    std::cout << "Values cannot contain '|'.\n";
                    break;
                }

                const std::string updated_record = build_plain_record(resource_name,
                                                                      description,
                                                                      user_name,
                                                                      password,
                                                                      password_last_modified);
                const std::size_t storage_index = selected.storage_index;
                records[storage_index] = encrypt_credential_record(updated_record, key);
                std::cout << "Updated record " << idx << ".\n";
                break;
            }
            case 4: {
                if (records.empty()) {
                    std::cout << "No records to delete.\n";
                    break;
                }

                std::size_t skipped = 0;
                const std::vector<DisplayRecord> display = build_sorted_decrypted_records(records, key, skipped);
                if (display.empty()) {
                    std::cout << "No readable records to delete with current master password.\n";
                    break;
                }

                print_decrypted_records(display, skipped);
                std::cout << "Enter index to delete (1-" << display.size() << "): ";
                const int idx = read_menu_choice();
                if (idx < 1 || static_cast<std::size_t>(idx) > display.size()) {
                    std::cout << "Invalid index.\n";
                    break;
                }

                const std::size_t storage_index = display[idx - 1].storage_index;
                records.erase(records.begin() + static_cast<std::ptrdiff_t>(storage_index));
                std::cout << "Deleted record " << idx << ".\n";
                break;
            }
            case 5:
                if (!save_records(vault_file, metadata.salt, metadata.auth_tag, records)) {
                    std::cerr << "Failed to save vault file: " << vault_file << "\n";
                    return 1;
                }
                std::cout << "Saved " << records.size() << " encrypted record(s) to " << vault_file << ".\n";
                break;
            case 6:
                std::cout << "Exiting without saving changes.\n";
                return 0;
            case 7:
                if (!save_records(vault_file, metadata.salt, metadata.auth_tag, records)) {
                    std::cerr << "Failed to save vault file: " << vault_file << "\n";
                    return 1;
                }
                std::cout << "Saved " << records.size() << " encrypted record(s) to " << vault_file << ".\n";
                return 0;
            default:
                std::cout << "Invalid choice.\n";
                break;
        }
    }

    return 0;
}
