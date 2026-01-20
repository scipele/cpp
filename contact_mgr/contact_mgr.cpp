//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | contact_manager.cpp                                         |
//| EntryPoint   | main                                                        |
//| Purpose      | merge and cleanup contacts from various sources             |
//| Inputs       | import from files                                           |
//| Outputs      | varies                                                      |
//| Dependencies | see standard includes                                       |
//| By Name,Date | T.Sciple, 01/19/2026                                        |

// ============================================================================
// Includes & Forward Declarations
// ============================================================================

#include <iostream>
#include <unordered_map>
#include <string>
#include <iomanip>
#include <vector>
#include <fstream>
#include <limits>
#include <climits>    // for INT_MAX
#include <cstdint>    // for int8_t, etc.
#include <algorithm>  // for transform (lowercase)
#include <set>        // for duplicate detection

#ifdef _WIN32
#include <conio.h>    // for _getch() on Windows
#else
#include <termios.h>  // for raw keyboard input (arrow keys)
#include <unistd.h>   // for read()
#endif

// Forward declarations
void clearScreen();
void waitForEnter();

// ============================================================================
// Enums
// ============================================================================

enum class PhoneType {
    Home,
    Cell,
    Work,
    School,
    Icloud
};


enum class EmailType {
    Personal,
    Home,
    Work,
    School,
    Other
};

// ============================================================================
// Data Structures
// ============================================================================

struct PhoneNumberAndType {
    std::string number;
    PhoneType type;
    std::string extension;  // Optional extension (e.g., "1105")
};


struct EmailAndType {
    std::string address;
    EmailType type;
};


struct Contact {
    int contact_id;
    std::string firstName;
    std::string middleName;
    std::string lastName;
    std::string company;
    std::string jobTitle; 
    std::vector<PhoneNumberAndType> phoneNumbers;
    std::vector<EmailAndType> emails;  // Changed from single string to vector
    std::string home_address, home_city, home_state, home_zip, home_country;
    std::string work_address, work_city, work_state, work_zip, work_country;
    std::string other_street, other_city, other_state, other_zip, other_country;
    std::string birthday;
    std::string anniversary;
    std::string notes;
    std::string website;
};

// ============================================================================
// ContactMgr Class Declaration
// ============================================================================

class ContactMgr {
private:
    std::vector<Contact> contacts;
    std::string getNextField(std::stringstream& ss);
    
    // Indexes for fast lookup
    std::unordered_map<std::string, std::vector<int>> nameIndex;    // "last|first" -> contact_ids
    std::unordered_map<std::string, int> phoneIndex;                // phone -> contact_id
    std::unordered_map<std::string, int> emailIndex;                // email -> contact_id

public:
    void add(const Contact& newContact);
    void viewAllContacts() const;
    void exportToPipeCSV(const std::string& filename);          // 3. Export to Pipe-Delimited CSV
    bool exportToVCF(const std::string& filename) const;        // 4. Export to VCF
    bool exportToTextFile(const std::string& filename) const;   // 5. Export formatted text
    void viewAllContacts(std::ostream& out) const;              // 6. Output to any stream

    void searchName(const std::string& firstName) const;
    size_t importFromVCF(const std::string& filename);
    size_t importFromYahooCSV(const std::string& filename);
    bool cleanAllPhoneNos();
    size_t removeDuplicatePhones();
    std::string show_phone_mask(const std::string& str) const;
    void display(const Contact& cn) const;
    void display(const Contact& cn, std::ostream& out) const;  // Output to any stream
    void out_pad(const std::string& strA,
                 const std::string& strB,
                 int wid) const;
    void out_pad(const std::string& strA,
                 const std::string& strB,
                 int wid,
                 std::ostream& out) const;  // Output to any stream
    std::string getPhoneTypeStr(const PhoneType& type) const;
    std::string getEmailTypeStr(const EmailType& type) const;
    int getNextId() const;
    
    // 6. View all contacts
    // 7. Search by name (exact) - searchName()
    // 8. Partial name search
    void searchNamePartial(const std::string& partial) const;
    
    // 9. Search by phone / 10. Search by email
    void rebuildIndexes();
    void searchByPhone(const std::string& phone) const;
    void searchByEmail(const std::string& email) const;
    bool isDuplicateName(const std::string& firstName, const std::string& lastName) const;
    std::string makeUniqueName(const std::string& firstName, const std::string& lastName) const;
    
    // 11. Add new contact / 12. Edit contact
    bool editContact(int contact_id);
    void addNewContact();
    
    // 13. Browse & delete
    size_t interactiveBrowseDelete();
    
    // 16. Find & merge duplicates
    std::vector<std::pair<int, int>> findDuplicates() const;
    size_t interactiveMergeDuplicates();
    
    // 17. Extract names from emails
    size_t extractNamesFromEmails();
    
    // 18. Bulk update email domain
    size_t bulkUpdateEmailDomain(const std::string& oldDomain, const std::string& newDomain);
    
    // 19. Renumber contact IDs
    void renumberContactIds();
    
    // 20. Save/Load functionality
    bool saveToFile(const std::string& filename) const;
    size_t loadFromFile(const std::string& filename);
    size_t getContactCount() const { return contacts.size(); }
    
private:
    PhoneType parsePhoneType(const std::string& typeStr) const;
    EmailType parseEmailType(const std::string& typeStr) const;
    std::string toLower(const std::string& str) const;
    std::string makeNameKey(const std::string& first, const std::string& last) const;
    void addToIndexes(const Contact& c);
    const Contact* findContactById(int id) const;
    std::string capitalizeFirst(const std::string& str) const;
    
    // Merge helpers
    Contact* findContactByIdMutable(int id);
    void removeContactById(int id);
    void displaySideBySide(const Contact& left, const Contact& right) const;
    std::string chooseField(const std::string& label, const std::string& left, const std::string& right) const;
    bool hasSharedPhone(const Contact& a, const Contact& b) const;
    bool hasSharedEmail(const Contact& a, const Contact& b) const;
    bool hasConflicts(const Contact& a, const Contact& b, std::vector<std::string>& conflictFields) const;
    void autoMergeInto(Contact& target, const Contact& source);
};


// ============================================================================
// Class Implementation - Core Methods
// ============================================================================

// ============================================================================
// 1. Import From VCF
// ============================================================================

size_t ContactMgr::importFromVCF(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return -1;
    }

    std::string line;
    Contact tempContact; // Use the name you declared
    bool inCard = false;
    int importedCount = 0;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (line == "BEGIN:VCARD") {
            importedCount++;
            inCard = true;
            tempContact = Contact(); // Reset tempContact
            tempContact.contact_id = getNextId();
        } else if (line == "END:VCARD") {
            if (inCard) {
                contacts.push_back(tempContact); // Successfully access private vector
                inCard = false;
            }
        } else if (inCard) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);

                // Map vCard keys to your struct members
                if (key.find("N") == 0) {
                    size_t firstSemi = value.find(';');
                    if (firstSemi != std::string::npos) {
                        tempContact.lastName = value.substr(0, firstSemi);
                        size_t secondSemi = value.find(';', firstSemi + 1);
                        tempContact.firstName = value.substr(firstSemi + 1, secondSemi - (firstSemi + 1));
                    }
                } else if (key.find("TEL;type=HOME") == 0) {
                    tempContact.phoneNumbers.push_back({value, PhoneType::Home, ""});
                } else if (key.find("TEL;type=CELL") == 0) {
                    tempContact.phoneNumbers.push_back({value, PhoneType::Cell, ""});
                } else if (key.find("TEL;type=WORK") == 0) {
                    tempContact.phoneNumbers.push_back({value, PhoneType::Work, ""});
                } else if (key.find("EMAIL;type=HOME") == 0) {
                    tempContact.emails.push_back({value, EmailType::Home});
                } else if (key.find("EMAIL;type=WORK") == 0) {
                    tempContact.emails.push_back({value, EmailType::Work});
                } else if (key.find("EMAIL") == 0) {
                    tempContact.emails.push_back({value, EmailType::Personal});
                }
            }
        }
    }
    file.close();
    
    // Rebuild indexes after import
    rebuildIndexes();
    
    return importedCount;
}


// ============================================================================
// 2. Import From Yahoo CSV
// ============================================================================
size_t ContactMgr::importFromYahooCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return -1;

    std::string line;
    // Skip the header row
    std::getline(file, line);
    
    int importedCount = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        importedCount++;
        
        std::stringstream ss(line);
        Contact c;

        // 1-5: Basic Info
        // Assign ID based on current vector size + 1
        c.contact_id = getNextId();

        c.firstName  = getNextField(ss);
        c.middleName = getNextField(ss);
        c.lastName   = getNextField(ss);
        c.company    = getNextField(ss);
        c.jobTitle   = getNextField(ss);

        // 6-8: Emails (now properly mapped to email types)
        std::string emailMain = getNextField(ss);
        std::string emailHome = getNextField(ss);
        std::string emailWork = getNextField(ss);
        
        // Map emails to the emails vector with proper types
        if (!emailMain.empty()) c.emails.push_back({emailMain, EmailType::Personal});
        if (!emailHome.empty()) c.emails.push_back({emailHome, EmailType::Home});
        if (!emailWork.empty()) c.emails.push_back({emailWork, EmailType::Work});

        // 9-15: Phone Numbers (Mapping Yahoo columns to your PhoneType)
        std::string pGen    = getNextField(ss); // General Phone
        std::string pHome   = getNextField(ss);
        std::string pWork   = getNextField(ss);
        std::string pPager  = getNextField(ss);
        std::string pFax    = getNextField(ss);
        std::string pMobile = getNextField(ss);
        std::string pOther  = getNextField(ss);

        if (!pHome.empty())   c.phoneNumbers.push_back({pHome,   PhoneType::Home, ""});
        if (!pMobile.empty()) c.phoneNumbers.push_back({pMobile, PhoneType::Cell, ""});
        if (!pWork.empty())   c.phoneNumbers.push_back({pWork,   PhoneType::Work, ""});
        if (!pGen.empty())    c.phoneNumbers.push_back({pGen,    PhoneType::Cell, ""});

        // 16-20: Home Address
        c.home_address = getNextField(ss);
        c.home_city    = getNextField(ss);
        c.home_state   = getNextField(ss);
        c.home_zip     = getNextField(ss);
        c.home_country = getNextField(ss);

        // 21-25: Work Address
        c.work_address = getNextField(ss);
        c.work_city    = getNextField(ss);
        c.work_state   = getNextField(ss);
        c.work_zip     = getNextField(ss);
        c.work_country = getNextField(ss);

        // 26-30: Other Address
        c.other_street  = getNextField(ss);
        c.other_city    = getNextField(ss);
        c.other_state   = getNextField(ss);
        c.other_zip     = getNextField(ss);
        c.other_country = getNextField(ss);

        // 31-34: Remaining fields
        c.birthday    = getNextField(ss);
        c.anniversary = getNextField(ss);
        c.notes       = getNextField(ss);
        c.website     = getNextField(ss);

        contacts.push_back(c);
    }
    
    // Rebuild indexes after import
    rebuildIndexes();
    
    return importedCount;
}


// ============================================================================
// 3. Export to CSV with Pipe Delimiter
// ============================================================================

void ContactMgr::exportToPipeCSV(const std::string& filename) {
std::ofstream outFile(filename);
if (!outFile.is_open()) {
    std::cerr << "Error: Could not create file " << filename << std::endl;
    return;
}

// Create a sorted copy of contacts (sort by last name, then first name)
std::vector<Contact> sorted = contacts;
std::sort(sorted.begin(), sorted.end(), [this](const Contact& a, const Contact& b) {
    std::string lastA = toLower(a.lastName);
    std::string lastB = toLower(b.lastName);
    if (lastA != lastB) return lastA < lastB;
    return toLower(a.firstName) < toLower(b.firstName);
});

// Optional: Add 'sep=|' to help Excel automatically recognize the delimiter
outFile << "sep=|" << "\n";

// 1. Write the Header Row
outFile << "First Name|Last Name|Phone Number|Email" << "\n";

// 2. Iterate and write data
for (const auto& contact : sorted) {
    outFile << contact.firstName << "|"
            << contact.lastName << "|";

            // Iterate through phone numbers and write them separated by commas
    for (size_t i = 0; i < contact.phoneNumbers.size(); ++i) {
        outFile << contact.phoneNumbers[i].number;
        if (i < contact.phoneNumbers.size() - 1) {
            outFile << ","; // Separate multiple phone numbers with commas
        }
    }
    outFile << "|";
    // Iterate through emails and write them separated by commas
    for (size_t i = 0; i < contact.emails.size(); ++i) {
        outFile << contact.emails[i].address;
        if (i < contact.emails.size() - 1) {
            outFile << ","; // Separate multiple emails with commas
        }
    }
    outFile << "\n";
}

outFile.close();
std::cout << "Data successfully exported to " << filename << std::endl;
}


// ============================================================================
// 4. Export contacts to VCF (vCard 3.0) format for iPhone/iCloud
// ============================================================================
bool ContactMgr::exportToVCF(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not create file " << filename << std::endl;
        return false;
    }
    
    // Create a sorted copy of contacts (sort by last name, then first name)
    std::vector<Contact> sorted = contacts;
    std::sort(sorted.begin(), sorted.end(), [this](const Contact& a, const Contact& b) {
        std::string lastA = toLower(a.lastName);
        std::string lastB = toLower(b.lastName);
        if (lastA != lastB) return lastA < lastB;
        return toLower(a.firstName) < toLower(b.firstName);
    });
    
    size_t exportedCount = 0;
    
    for (const auto& contact : sorted) {
        // Skip contacts with no useful data
        if (contact.firstName.empty() && contact.lastName.empty() && 
            contact.phoneNumbers.empty() && contact.emails.empty()) {
            continue;
        }
        
        outFile << "BEGIN:VCARD\n";
        outFile << "VERSION:3.0\n";
        
        // N: Last;First;Middle;Prefix;Suffix
        outFile << "N:" << contact.lastName << ";" << contact.firstName << ";"
                << contact.middleName << ";;\n";
        
        // FN: Full Name (required in vCard 3.0)
        std::string fullName;
        if (!contact.firstName.empty()) fullName = contact.firstName;
        if (!contact.middleName.empty()) {
            if (!fullName.empty()) fullName += " ";
            fullName += contact.middleName;
        }
        if (!contact.lastName.empty()) {
            if (!fullName.empty()) fullName += " ";
            fullName += contact.lastName;
        }
        if (fullName.empty()) fullName = "Unknown";
        outFile << "FN:" << fullName << "\n";
        
        // ORG: Company
        if (!contact.company.empty()) {
            outFile << "ORG:" << contact.company << "\n";
        }
        
        // TITLE: Job Title
        if (!contact.jobTitle.empty()) {
            outFile << "TITLE:" << contact.jobTitle << "\n";
        }
        
        // TEL: Phone numbers with types
        for (const auto& phone : contact.phoneNumbers) {
            if (phone.number.empty()) continue;
            
            std::string vcfType;
            switch (phone.type) {
                case PhoneType::Home:   vcfType = "HOME,VOICE"; break;
                case PhoneType::Cell:   vcfType = "CELL"; break;
                case PhoneType::Work:   vcfType = "WORK,VOICE"; break;
                case PhoneType::School: vcfType = "VOICE"; break;
                case PhoneType::Icloud: vcfType = "CELL"; break;
                default:                vcfType = "VOICE"; break;
            }
            
            // Format phone number for vCard (with + prefix for international)
            std::string formattedPhone = phone.number;
            if (formattedPhone.length() == 11 && formattedPhone[0] == '1') {
                // US format: +1 (XXX) XXX-XXXX
                formattedPhone = "+" + formattedPhone.substr(0,1) + " (" + 
                                 formattedPhone.substr(1,3) + ") " +
                                 formattedPhone.substr(4,3) + "-" +
                                 formattedPhone.substr(7,4);
            } else if (formattedPhone.length() == 10) {
                // US format without country code
                formattedPhone = "+1 (" + formattedPhone.substr(0,3) + ") " +
                                 formattedPhone.substr(3,3) + "-" +
                                 formattedPhone.substr(6,4);
            }
            
            // Add extension if present
            if (!phone.extension.empty()) {
                formattedPhone += " x" + phone.extension;
            }
            
            outFile << "TEL;TYPE=" << vcfType << ":" << formattedPhone << "\n";
        }
        
        // EMAIL: Email addresses with types
        for (const auto& email : contact.emails) {
            if (email.address.empty()) continue;
            
            std::string vcfType;
            switch (email.type) {
                case EmailType::Personal: vcfType = "INTERNET"; break;
                case EmailType::Home:     vcfType = "HOME,INTERNET"; break;
                case EmailType::Work:     vcfType = "WORK,INTERNET"; break;
                case EmailType::School:   vcfType = "INTERNET"; break;
                case EmailType::Other:    vcfType = "INTERNET"; break;
                default:                  vcfType = "INTERNET"; break;
            }
            
            outFile << "EMAIL;TYPE=" << vcfType << ":" << email.address << "\n";
        }
        
        // ADR: Home address
        if (!contact.home_address.empty() || !contact.home_city.empty() ||
            !contact.home_state.empty() || !contact.home_zip.empty()) {
            // ADR: POBox;Extended;Street;City;State;Zip;Country
            outFile << "ADR;TYPE=HOME:;;" << contact.home_address << ";"
                    << contact.home_city << ";" << contact.home_state << ";"
                    << contact.home_zip << ";" << contact.home_country << "\n";
        }
        
        // ADR: Work address
        if (!contact.work_address.empty() || !contact.work_city.empty() ||
            !contact.work_state.empty() || !contact.work_zip.empty()) {
            outFile << "ADR;TYPE=WORK:;;" << contact.work_address << ";"
                    << contact.work_city << ";" << contact.work_state << ";"
                    << contact.work_zip << ";" << contact.work_country << "\n";
        }
        
        // BDAY: Birthday
        if (!contact.birthday.empty()) {
            outFile << "BDAY:" << contact.birthday << "\n";
        }
        
        // URL: Website
        if (!contact.website.empty()) {
            outFile << "URL:" << contact.website << "\n";
        }
        
        // NOTE: Notes
        if (!contact.notes.empty()) {
            // Escape newlines in notes for vCard format
            std::string escapedNotes = contact.notes;
            size_t pos = 0;
            while ((pos = escapedNotes.find('\n', pos)) != std::string::npos) {
                escapedNotes.replace(pos, 1, "\\n");
                pos += 2;
            }
            outFile << "NOTE:" << escapedNotes << "\n";
        }
        
        outFile << "END:VCARD\n";
        exportedCount++;
    }
    
    outFile.close();
    std::cout << "Exported " << exportedCount << " contacts to " << filename << std::endl;
    return true;
}


// Helper to safely extract CSV fields
std::string ContactMgr::getNextField(std::stringstream& ss) {
    std::string field;
    if (!std::getline(ss, field, ',')) return "";
    // Optional: Remove quotes if ` exports them (e.g., "John")
    // if (!field.empty() && field.front() == '"') {
    //     field = field.substr(1, field.size() - 2);
    // }
    return field;
}


// ============================================================================
// 5. Export to Formatted Text File
//  ===========================================================================
bool ContactMgr::exportToTextFile(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not create file " << filename << std::endl;
        return false;
    }
    
    // Add header with timestamp
    outFile << "Contact Manager Export" << std::endl;
    outFile << "Generated: " << __DATE__ << " " << __TIME__ << std::endl;
    outFile << "Total Contacts: " << contacts.size() << std::endl;
    
    viewAllContacts(outFile);
    
    outFile.close();
    std::cout << "Exported " << contacts.size() << " contacts to " << filename << std::endl;
    return true;
}


// ============================================================================
// 11. Method to add a new contact (with duplicate detection)
// ============================================================================
void ContactMgr::add(const Contact& newContact) {
    Contact c = newContact;
    
    // Check for duplicate name
    if (isDuplicateName(c.firstName, c.lastName)) {
        std::string uniqueFirst = makeUniqueName(c.firstName, c.lastName);
        std::cout << "Warning: Contact '" << c.firstName << " " << c.lastName 
                  << "' already exists. Saving as '" << uniqueFirst << " " << c.lastName << "'" << std::endl;
        c.firstName = uniqueFirst;
    }
    
    contacts.push_back(c);
    addToIndexes(c);
    std::cout << "Contact added successfully." << std::endl;
}


// ============================================================================
// 14. Phone Number Processing
// ============================================================================

bool ContactMgr::cleanAllPhoneNos() {
    // Outer loop: Iterate through every contact in the manager
    if(contacts.empty()) return false;
    
    size_t extensionsFound = 0;
    
    for (auto& contact : contacts) {
        
        // Inner loop: Iterate through every PhoneNumber struct in the contact's vector
        for (auto& pEntry : contact.phoneNumbers) {
            
            std::string cur_no = pEntry.number;
            std::string cleaned;

            // 1. Character-by-character cleaning (keep only digits)
            for (char c : cur_no) {
                if (isdigit(c)) {
                    cleaned += c;
                }
            }

            // 2. Handle extensions - numbers longer than 11 digits
            // Standard US number: 10 digits (or 11 with country code)
            // If longer, the extra digits are likely an extension
            std::string extension;
            
            if (cleaned.length() > 11) {
                // Extract extension (digits after the 10-digit phone number)
                
                if (cleaned.length() >= 14) {
                    // Format: 40983278271105 -> 14098327827 (11 digits) + 1105 (ext)
                    // Check if starts with country code
                    if (cleaned[0] == '1') {
                        extension = cleaned.substr(11);
                        cleaned = cleaned.substr(0, 11);
                    } else {
                        // No country code: 4098327827 (10 digits) + 1105 (ext)
                        extension = cleaned.substr(10);
                        cleaned = cleaned.substr(0, 10);
                    }
                } else {
                    // 12-13 digits: likely 10 or 11 digit number + short extension
                    if (cleaned[0] == '1') {
                        extension = cleaned.substr(11);
                        cleaned = cleaned.substr(0, 11);
                    } else {
                        extension = cleaned.substr(10);
                        cleaned = cleaned.substr(0, 10);
                    }
                }
            }
            
            // 3. Add country code if 10 digits
            if (cleaned.length() == 10) {
                cleaned = "1" + cleaned;
            }

            // 4. Save the cleaned string back to the specific phone entry
            pEntry.number = cleaned;
            
            // 5. Store extension in the struct field
            if (!extension.empty()) {
                pEntry.extension = extension;
                extensionsFound++;
                std::cout << "  Found extension: " << show_phone_mask(cleaned) << " x" << extension << std::endl;
            }
        }
    }
    
    if (extensionsFound > 0) {
        std::cout << "Extracted " << extensionsFound << " extension(s)." << std::endl;
    }
    
    // Rebuild phone index since numbers changed
    rebuildIndexes();

    return true;
}


// ============================================================================
// 15. Remove duplicate phone numbers within each contact
// ============================================================================
size_t ContactMgr::removeDuplicatePhones() {
    if (contacts.empty()) return 0;
    
    size_t totalRemoved = 0;
    size_t contactsAffected = 0;
    
    for (auto& contact : contacts) {
        if (contact.phoneNumbers.size() <= 1) continue;  // No duplicates possible
        
        std::set<std::string> seenPhones;
        std::vector<PhoneNumberAndType> uniquePhones;
        size_t removedFromContact = 0;
        
        for (const auto& phone : contact.phoneNumbers) {
            // Normalize: use just the digits for comparison
            std::string normalized = phone.number;
            
            if (seenPhones.find(normalized) == seenPhones.end()) {
                // First occurrence - keep it
                seenPhones.insert(normalized);
                uniquePhones.push_back(phone);
            } else {
                // Duplicate - skip it
                removedFromContact++;
                std::cout << "  Removed duplicate: " << show_phone_mask(phone.number)
                          << " from " << contact.firstName << " " << contact.lastName << std::endl;
            }
        }
        
        if (removedFromContact > 0) {
            contact.phoneNumbers = uniquePhones;
            totalRemoved += removedFromContact;
            contactsAffected++;
        }
    }
    
    // Rebuild phone index since phones were removed
    if (totalRemoved > 0) {
        rebuildIndexes();
        std::cout << "\nSummary: Removed " << totalRemoved << " duplicate phone(s) from " 
                  << contactsAffected << " contact(s)." << std::endl;
    }
    
    return totalRemoved;
}


// ============================================================================
// Helper Function to Get Next Contact ID
// ============================================================================
int ContactMgr::getNextId() const {
    int maxId = 0;
    for (const auto& c : contacts) {
        if (c.contact_id > maxId) {
            maxId = c.contact_id;
        }
    }
    return maxId + 1;
}


































// ============================================================================
// Display & Formatting Helpers
// ============================================================================

// Method to view all contacts (screen version - calls ostream version)
void ContactMgr::viewAllContacts() const {
    viewAllContacts(std::cout);
}

// Method to view all contacts (sorted alphabetically by last name, then first name)
// Outputs to any ostream (screen or file)
void ContactMgr::viewAllContacts(std::ostream& out) const {
    if (contacts.empty()) {
        out << "No contacts found." << std::endl;
        return;
    }
    
    // Create a sorted copy of contacts (sort by last name, then first name)
    std::vector<Contact> sorted = contacts;
    std::sort(sorted.begin(), sorted.end(), [this](const Contact& a, const Contact& b) {
        std::string lastA = toLower(a.lastName);
        std::string lastB = toLower(b.lastName);
        if (lastA != lastB) return lastA < lastB;
        return toLower(a.firstName) < toLower(b.firstName);
    });
    
    out << "\n";
    char currentLetter = '\0';
    
    for (const auto& contact : sorted) {
        // Determine the first letter of last name (or first name if no last name)
        char firstChar = '\0';
        if (!contact.lastName.empty()) {
            firstChar = static_cast<char>(std::toupper(static_cast<unsigned char>(contact.lastName[0])));
        } else if (!contact.firstName.empty()) {
            firstChar = static_cast<char>(std::toupper(static_cast<unsigned char>(contact.firstName[0])));
        }
        
        // Print letter header when letter changes
        if (firstChar != currentLetter && std::isalpha(static_cast<unsigned char>(firstChar))) {
            currentLetter = firstChar;
            out << "==================================================" << std::endl;
            out << std::string(22, ' ') << "--" << currentLetter << "--" << std::endl;
            out << "==================================================" << std::endl;
        } else if (firstChar != currentLetter && !std::isalpha(static_cast<unsigned char>(firstChar)) && currentLetter != '#') {
            // Non-alphabetic characters grouped under #
            currentLetter = '#';
            out << "==================================================" << std::endl;
            out << std::string(22, ' ') << "--#--" << std::endl;
            out << "==================================================" << std::endl;
        }
        
        display(contact, out);        
    }
    out << "==================================================" << std::endl;
    out << "Total: " << contacts.size() << " contacts" << std::endl;
}




std::string ContactMgr::show_phone_mask(const std::string& str) const{
    int len = str.length();

    std::string tmp = str;

    switch (len)
    {
    case 7:
        return tmp.substr(0,3) + "-" + tmp.substr(3,4);
    case 11:
        return "+" + tmp.substr(0,1) + " (" +  tmp.substr(1,3) + ") " + tmp.substr(4,3) + "-" + tmp.substr(7,4);
    case 12:
        return "+" + tmp.substr(0,2) + " (" +  tmp.substr(2,3) + ") " + tmp.substr(5,3) + "-" + tmp.substr(8,4);
    case 10:
        return "(" +  tmp.substr(0,3) + ") " + tmp.substr(3,3) + "-" + tmp.substr(6,4);
        break;
    default:
        return tmp;
        break;
    } 
}


void ContactMgr::out_pad(const std::string& strA,
                         const std::string& strB,
                         int wid) const {
    out_pad(strA, strB, wid, std::cout);
}

void ContactMgr::out_pad(const std::string& strA,
                         const std::string& strB,
                         int wid,
                         std::ostream& out) const {
    out << std::setw(wid) << std::left << strA << strB << "\n";
}


std::string ContactMgr::getPhoneTypeStr(const PhoneType& type) const{
    switch (type) {
        case PhoneType::Home:   return "Home";
        case PhoneType::Cell:   return "Cell";
        case PhoneType::Work:   return "Work";
        case PhoneType::School: return "School";
    }
    return "Unknown";
}


std::string ContactMgr::getEmailTypeStr(const EmailType& type) const {
    switch (type) {
        case EmailType::Personal: return "Personal";
        case EmailType::Home:     return "Home";
        case EmailType::Work:     return "Work";
        case EmailType::School:   return "School";
        case EmailType::Other:    return "Other";
    }
    return "Unknown";
}


void ContactMgr::display(const Contact& cn) const {
    display(cn, std::cout);
}

void ContactMgr::display(const Contact& cn, std::ostream& out) const {
    out_pad("contact_id:", std::to_string(cn.contact_id), 15, out);
    if (!cn.firstName.empty()) out_pad("First_Name:", cn.firstName, 15, out);
    if (!cn.lastName.empty()) out_pad("Last_Name:", cn.lastName, 15, out);   
    
        // Inner loop: Iterate through every PhoneNumber struct in the contact's vector
        if (!cn.phoneNumbers.empty()) {
            out << "Phone Numbers:"; 
            int8_t count = 0;
            std::string indent = " "; // Initial indent for the first number
            for (const auto& pEntry : cn.phoneNumbers) {
                if (count++ > 0) indent = "               "; // Indent for subsequent numbers
                std::string type_str = getPhoneTypeStr(pEntry.type);
                std::string phone_display = show_phone_mask(pEntry.number);
                // Append extension if present
                if (!pEntry.extension.empty()) {
                    phone_display += " x" + pEntry.extension;
                }
                out_pad(indent + type_str, phone_display, indent.length() + 8, out);
            }
        }
        // Display emails with types
        if (!cn.emails.empty()) {
            out << "Email(s):      "; 
            int8_t count = 0;
            std::string indent = ""; // Initial indent for the first email
            for (const auto& eEntry : cn.emails) {
                if (count++ > 0) indent = "               "; // Indent for subsequent emails
                std::string type_str = getEmailTypeStr(eEntry.type);
                out_pad(indent + type_str + ": ", eEntry.address, indent.length() + 10, out);
            }
        }
        out << "----------------------------------------------\n" << std::endl;
} 

// ============================================================================
// 7. Search Functions
// ============================================================================

// Method to search for a contact by name (using index)
void ContactMgr::searchName(const std::string& last_comma_first_name) const {
    size_t commaPos = last_comma_first_name.find(",");
    std::string first_name, last_name;
    
    if (commaPos != std::string::npos) {
        last_name = last_comma_first_name.substr(0, commaPos);
        first_name = last_comma_first_name.substr(commaPos + 1);
        // Trim leading space from first name if present
        if (!first_name.empty() && first_name[0] == ' ') {
            first_name = first_name.substr(1);
        }
    } else {
        // Assume it's just a last name
        last_name = last_comma_first_name;
    }
    
    std::string key = makeNameKey(first_name, last_name);
    auto it = nameIndex.find(key);
    
    if (it != nameIndex.end() && !it->second.empty()) {
        std::cout << "Found " << it->second.size() << " contact(s):" << std::endl;
        for (int id : it->second) {
            const Contact* c = findContactById(id);
            if (c) display(*c);
        }
    } else {
        // Fallback: partial match on last name only
        bool found = false;
        std::string lowerLast = toLower(last_name);
        for (const auto& contact : contacts) {
            if (toLower(contact.lastName) == lowerLast) {
                display(contact);
                found = true;
            }
        }
        if (!found) {
            std::cout << "No contact found with name: " << last_comma_first_name << std::endl;
        }
    }
}

// ============================================================================
// 8. Search by Partial Name
// ============================================================================

// Search for contacts where any part of first or last name matches
void ContactMgr::searchNamePartial(const std::string& partial) const {
    if (partial.empty()) {
        std::cout << "Please enter a search term." << std::endl;
        return;
    }
    
    std::string lowerPartial = toLower(partial);
    std::vector<const Contact*> matches;
    
    for (const auto& contact : contacts) {
        std::string lowerFirst = toLower(contact.firstName);
        std::string lowerLast = toLower(contact.lastName);
        
        // Check if partial appears anywhere in first or last name
        if (lowerFirst.find(lowerPartial) != std::string::npos ||
            lowerLast.find(lowerPartial) != std::string::npos) {
            matches.push_back(&contact);
        }
    }
    
    if (matches.empty()) {
        std::cout << "No contacts found matching \"" << partial << "\"" << std::endl;
        return;
    }
    
    // Sort matches alphabetically
    std::sort(matches.begin(), matches.end(), 
        [this](const Contact* a, const Contact* b) {
            std::string lastA = toLower(a->lastName);
            std::string lastB = toLower(b->lastName);
            if (lastA != lastB) return lastA < lastB;
            return toLower(a->firstName) < toLower(b->firstName);
        });
    
    std::cout << "\nFound " << matches.size() << " contact(s) matching \"" << partial << "\":\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (const Contact* c : matches) {
        display(*c);
    }
}


// ============================================================================
// 9. Search by phone number (fast indexed lookup)
// ============================================================================
void ContactMgr::searchByPhone(const std::string& phone) const {
    // Clean the input phone number (digits only)
    std::string cleaned;
    for (char c : phone) {
        if (isdigit(c)) cleaned += c;
    }
    
    // Try exact match first
    auto it = phoneIndex.find(cleaned);
    if (it != phoneIndex.end()) {
        const Contact* c = findContactById(it->second);
        if (c) {
            std::cout << "Found contact by phone:" << std::endl;
            display(*c);
            return;
        }
    }
    
    // Also try with country code prepended
    if (cleaned.length() == 10) {
        auto it2 = phoneIndex.find("1" + cleaned);
        if (it2 != phoneIndex.end()) {
            const Contact* c = findContactById(it2->second);
            if (c) {
                std::cout << "Found contact by phone:" << std::endl;
                display(*c);
                return;
            }
        }
    }
    
    std::cout << "No contact found with phone: " << phone << std::endl;
}

// ============================================================================
// 10. Search by email (fast indexed lookup)
// ============================================================================
void ContactMgr::searchByEmail(const std::string& email) const {
    std::string lowerEmail = toLower(email);
    
    auto it = emailIndex.find(lowerEmail);
    if (it != emailIndex.end()) {
        const Contact* c = findContactById(it->second);
        if (c) {
            std::cout << "Found contact by email:" << std::endl;
            display(*c);
            return;
        }
    }
    
    std::cout << "No contact found with email: " << email << std::endl;
}


// ============================================================================
// Ref 7-10. Indexing & Fast Search Helpers
// ============================================================================

// Convert string to lowercase for case-insensitive comparison
std::string ContactMgr::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}


// Create a normalized key from first and last name
std::string ContactMgr::makeNameKey(const std::string& first, const std::string& last) const {
    return toLower(last) + "|" + toLower(first);
}


// Find contact by ID
const Contact* ContactMgr::findContactById(int id) const {
    for (const auto& c : contacts) {
        if (c.contact_id == id) return &c;
    }
    return nullptr;
}


// ============================================================================
// 11. Add New Contact
// ============================================================================
// Add a new contact manually (menu-driven)
void ContactMgr::addNewContact() {
    Contact c;
    c.contact_id = getNextId();
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "\n=== Add New Contact ===\n";
    std::cout << "Press Enter to skip any field.\n\n";
    
    std::cout << "First Name: ";
    std::getline(std::cin, c.firstName);
    
    std::cout << "Middle Name: ";
    std::getline(std::cin, c.middleName);
    
    std::cout << "Last Name: ";
    std::getline(std::cin, c.lastName);
    
    std::cout << "Company: ";
    std::getline(std::cin, c.company);
    
    std::cout << "Job Title: ";
    std::getline(std::cin, c.jobTitle);
    
    // Add phone numbers
    std::cout << "\n--- Phone Numbers ---\n";
    char addMore = 'y';
    while (std::tolower(addMore) == 'y') {
        std::cout << "Phone number (digits only, or Enter to skip): ";
        std::string phone;
        std::getline(std::cin, phone);
        
        if (!phone.empty()) {
            // Clean the phone number
            std::string cleaned;
            for (char ch : phone) {
                if (std::isdigit(ch)) cleaned += ch;
            }
            
            if (!cleaned.empty()) {
                PhoneNumberAndType pn;
                pn.number = cleaned;
                
                std::cout << "Type (1=Home, 2=Cell, 3=Work, 4=School) [2]: ";
                std::string typeStr;
                std::getline(std::cin, typeStr);
                
                if (typeStr == "1") pn.type = PhoneType::Home;
                else if (typeStr == "3") pn.type = PhoneType::Work;
                else if (typeStr == "4") pn.type = PhoneType::School;
                else pn.type = PhoneType::Cell;
                
                std::cout << "Extension (or Enter for none): ";
                std::getline(std::cin, pn.extension);
                
                c.phoneNumbers.push_back(pn);
                std::cout << "Added: " << show_phone_mask(cleaned) << "\n";
            }
        }
        
        std::cout << "Add another phone? (y/n): ";
        std::string response;
        std::getline(std::cin, response);
        addMore = response.empty() ? 'n' : response[0];
    }
    
    // Add emails
    std::cout << "\n--- Email Addresses ---\n";
    addMore = 'y';
    while (std::tolower(addMore) == 'y') {
        std::cout << "Email address (or Enter to skip): ";
        std::string email;
        std::getline(std::cin, email);
        
        if (!email.empty()) {
            EmailAndType em;
            em.address = email;
            
            std::cout << "Type (1=Personal, 2=Home, 3=Work, 4=School, 5=Other) [1]: ";
            std::string typeStr;
            std::getline(std::cin, typeStr);
            
            if (typeStr == "2") em.type = EmailType::Home;
            else if (typeStr == "3") em.type = EmailType::Work;
            else if (typeStr == "4") em.type = EmailType::School;
            else if (typeStr == "5") em.type = EmailType::Other;
            else em.type = EmailType::Personal;
            
            c.emails.push_back(em);
            std::cout << "Added: " << email << "\n";
        }
        
        std::cout << "Add another email? (y/n): ";
        std::string response;
        std::getline(std::cin, response);
        addMore = response.empty() ? 'n' : response[0];
    }
    
    // Optional fields
    std::cout << "\n--- Additional Info (press Enter to skip) ---\n";
    
    std::cout << "Birthday (YYYY-MM-DD): ";
    std::getline(std::cin, c.birthday);
    
    std::cout << "Website: ";
    std::getline(std::cin, c.website);
    
    std::cout << "Notes: ";
    std::getline(std::cin, c.notes);
    
    // Confirm and add
    std::cout << "\n--- Review ---\n";
    display(c);
    
    std::cout << "\nSave this contact? (y/n): ";
    char confirm;
    std::cin >> confirm;
    
    if (std::tolower(confirm) == 'y') {
        contacts.push_back(c);
        addToIndexes(c);
        std::cout << "\nContact added successfully! (ID: " << c.contact_id << ")\n";
    } else {
        std::cout << "\nContact not saved.\n";
    }
}

// ============================================================================
// 12. Edit Contact
// ============================================================================
bool ContactMgr::editContact(int contact_id) {
    Contact* c = findContactByIdMutable(contact_id);
    if (!c) {
        std::cout << "Contact not found with ID: " << contact_id << std::endl;
        return false;
    }
    
    bool modified = false;
    char choice;
    
    do {
        clearScreen();
        std::cout << "\n=== Editing Contact (ID: " << c->contact_id << ") ===\n";
        std::cout << std::string(50, '-') << "\n";
        
        std::cout << " 1. First Name:  " << (c->firstName.empty() ? "(empty)" : c->firstName) << "\n";
        std::cout << " 2. Middle Name: " << (c->middleName.empty() ? "(empty)" : c->middleName) << "\n";
        std::cout << " 3. Last Name:   " << (c->lastName.empty() ? "(empty)" : c->lastName) << "\n";
        std::cout << " 4. Company:     " << (c->company.empty() ? "(empty)" : c->company) << "\n";
        std::cout << " 5. Job Title:   " << (c->jobTitle.empty() ? "(empty)" : c->jobTitle) << "\n";
        std::cout << std::string(50, '-') << "\n";
        
        // Show phones
        std::cout << " 6. Phone Numbers (" << c->phoneNumbers.size() << "):\n";
        for (size_t i = 0; i < c->phoneNumbers.size(); ++i) {
            std::cout << "      " << (i+1) << ". " << show_phone_mask(c->phoneNumbers[i].number)
                      << " (" << getPhoneTypeStr(c->phoneNumbers[i].type) << ")";
            if (!c->phoneNumbers[i].extension.empty()) {
                std::cout << " x" << c->phoneNumbers[i].extension;
            }
            std::cout << "\n";
        }
        
        // Show emails
        std::cout << " 7. Email Addresses (" << c->emails.size() << "):\n";
        for (size_t i = 0; i < c->emails.size(); ++i) {
            std::cout << "      " << (i+1) << ". " << c->emails[i].address
                      << " (" << getEmailTypeStr(c->emails[i].type) << ")\n";
        }
        
        std::cout << std::string(50, '-') << "\n";
        std::cout << " 8. Birthday:    " << (c->birthday.empty() ? "(empty)" : c->birthday) << "\n";
        std::cout << " 9. Website:     " << (c->website.empty() ? "(empty)" : c->website) << "\n";
        std::cout << "10. Notes:       " << (c->notes.empty() ? "(empty)" : c->notes) << "\n";
        std::cout << std::string(50, '-') << "\n";
        std::cout << " S. Save and exit\n";
        std::cout << " Q. Quit without saving\n";
        std::cout << std::string(50, '-') << "\n";
        std::cout << "Choice: ";
        
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        choice = std::toupper(choice);
        
        std::string newValue;
        
        switch (choice) {
            case '1':
                std::cout << "New First Name (Enter to keep current): ";
                std::getline(std::cin, newValue);
                if (!newValue.empty()) { c->firstName = newValue; modified = true; }
                break;
            case '2':
                std::cout << "New Middle Name (Enter to keep current): ";
                std::getline(std::cin, newValue);
                if (!newValue.empty()) { c->middleName = newValue; modified = true; }
                break;
            case '3':
                std::cout << "New Last Name (Enter to keep current): ";
                std::getline(std::cin, newValue);
                if (!newValue.empty()) { c->lastName = newValue; modified = true; }
                break;
            case '4':
                std::cout << "New Company (Enter to keep current): ";
                std::getline(std::cin, newValue);
                if (!newValue.empty()) { c->company = newValue; modified = true; }
                break;
            case '5':
                std::cout << "New Job Title (Enter to keep current): ";
                std::getline(std::cin, newValue);
                if (!newValue.empty()) { c->jobTitle = newValue; modified = true; }
                break;
            case '6':
                // Phone management submenu
                {
                    std::cout << "\nPhone Options:\n";
                    std::cout << "  A = Add new phone\n";
                    std::cout << "  D = Delete a phone\n";
                    std::cout << "  Enter = Cancel\n";
                    std::cout << "Choice: ";
                    std::string phoneChoice;
                    std::getline(std::cin, phoneChoice);
                    
                    if (!phoneChoice.empty() && std::toupper(phoneChoice[0]) == 'A') {
                        PhoneNumberAndType pn;
                        std::cout << "Phone number (digits only): ";
                        std::string phone;
                        std::getline(std::cin, phone);
                        
                        // Clean
                        for (char ch : phone) {
                            if (std::isdigit(ch)) pn.number += ch;
                        }
                        
                        if (!pn.number.empty()) {
                            std::cout << "Type (1=Home, 2=Cell, 3=Work, 4=School) [2]: ";
                            std::string typeStr;
                            std::getline(std::cin, typeStr);
                            
                            if (typeStr == "1") pn.type = PhoneType::Home;
                            else if (typeStr == "3") pn.type = PhoneType::Work;
                            else if (typeStr == "4") pn.type = PhoneType::School;
                            else pn.type = PhoneType::Cell;
                            
                            std::cout << "Extension (or Enter for none): ";
                            std::getline(std::cin, pn.extension);
                            
                            c->phoneNumbers.push_back(pn);
                            modified = true;
                            std::cout << "Phone added!\n";
                        }
                    } else if (!phoneChoice.empty() && std::toupper(phoneChoice[0]) == 'D') {
                        if (c->phoneNumbers.empty()) {
                            std::cout << "No phones to delete.\n";
                        } else {
                            std::cout << "Enter number to delete (1-" << c->phoneNumbers.size() << "): ";
                            int delIdx;
                            std::cin >> delIdx;
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            
                            if (delIdx >= 1 && delIdx <= static_cast<int>(c->phoneNumbers.size())) {
                                c->phoneNumbers.erase(c->phoneNumbers.begin() + delIdx - 1);
                                modified = true;
                                std::cout << "Phone deleted!\n";
                            }
                        }
                    }
                }
                std::cout << "Press Enter to continue...";
                std::cin.get();
                break;
            case '7':
                // Email management submenu
                {
                    std::cout << "\nEmail Options:\n";
                    std::cout << "  A = Add new email\n";
                    std::cout << "  D = Delete an email\n";
                    std::cout << "  Enter = Cancel\n";
                    std::cout << "Choice: ";
                    std::string emailChoice;
                    std::getline(std::cin, emailChoice);
                    
                    if (!emailChoice.empty() && std::toupper(emailChoice[0]) == 'A') {
                        EmailAndType em;
                        std::cout << "Email address: ";
                        std::getline(std::cin, em.address);
                        
                        if (!em.address.empty()) {
                            std::cout << "Type (1=Personal, 2=Home, 3=Work, 4=School, 5=Other) [1]: ";
                            std::string typeStr;
                            std::getline(std::cin, typeStr);
                            
                            if (typeStr == "2") em.type = EmailType::Home;
                            else if (typeStr == "3") em.type = EmailType::Work;
                            else if (typeStr == "4") em.type = EmailType::School;
                            else if (typeStr == "5") em.type = EmailType::Other;
                            else em.type = EmailType::Personal;
                            
                            c->emails.push_back(em);
                            modified = true;
                            std::cout << "Email added!\n";
                        }
                    } else if (!emailChoice.empty() && std::toupper(emailChoice[0]) == 'D') {
                        if (c->emails.empty()) {
                            std::cout << "No emails to delete.\n";
                        } else {
                            std::cout << "Enter number to delete (1-" << c->emails.size() << "): ";
                            int delIdx;
                            std::cin >> delIdx;
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            
                            if (delIdx >= 1 && delIdx <= static_cast<int>(c->emails.size())) {
                                c->emails.erase(c->emails.begin() + delIdx - 1);
                                modified = true;
                                std::cout << "Email deleted!\n";
                            }
                        }
                    }
                }
                std::cout << "Press Enter to continue...";
                std::cin.get();
                break;
            case '8':
                std::cout << "New Birthday (YYYY-MM-DD, Enter to keep current): ";
                std::getline(std::cin, newValue);
                if (!newValue.empty()) { c->birthday = newValue; modified = true; }
                break;
            case '9':
                std::cout << "New Website (Enter to keep current): ";
                std::getline(std::cin, newValue);
                if (!newValue.empty()) { c->website = newValue; modified = true; }
                break;
            case '0':  // 10
                std::cout << "New Notes (Enter to keep current): ";
                std::getline(std::cin, newValue);
                if (!newValue.empty()) { c->notes = newValue; modified = true; }
                break;
            case 'S':
                if (modified) {
                    rebuildIndexes();
                    std::cout << "Contact saved!\n";
                }
                return modified;
            case 'Q':
                if (modified) {
                    std::cout << "Discard changes? (y/n): ";
                    char discard;
                    std::cin >> discard;
                    if (std::tolower(discard) != 'y') {
                        continue;
                    }
                }
                return false;
        }
    } while (true);
    
    return modified;
}


// ============================================================================
// 16. Find & Merge Duplicate Contacts
// ============================================================================

// Helper: Find a contact by ID (mutable version for merging)
Contact* ContactMgr::findContactByIdMutable(int id) {
    for (auto& c : contacts) {
        if (c.contact_id == id) return &c;
    }
    return nullptr;
}

// Helper: Remove a contact by ID
void ContactMgr::removeContactById(int id) {
    contacts.erase(
        std::remove_if(contacts.begin(), contacts.end(),
            [id](const Contact& c) { return c.contact_id == id; }),
        contacts.end()
    );
}

// Helper: Check if two contacts share any phone number
bool ContactMgr::hasSharedPhone(const Contact& a, const Contact& b) const {
    for (const auto& pa : a.phoneNumbers) {
        for (const auto& pb : b.phoneNumbers) {
            if (pa.number == pb.number && !pa.number.empty()) {
                return true;
            }
        }
    }
    return false;
}

// Helper: Check if two contacts share any email
bool ContactMgr::hasSharedEmail(const Contact& a, const Contact& b) const {
    for (const auto& ea : a.emails) {
        for (const auto& eb : b.emails) {
            std::string emailA = toLower(ea.address);
            std::string emailB = toLower(eb.address);
            if (emailA == emailB && !emailA.empty()) {
                return true;
            }
        }
    }
    return false;
}

// Helper: Check if two contacts have conflicting data (both non-empty but different)
bool ContactMgr::hasConflicts(const Contact& a, const Contact& b, std::vector<std::string>& conflictFields) const {
    conflictFields.clear();
    
    // Helper lambda to check string fields
    auto checkField = [&](const std::string& fieldName, const std::string& valA, const std::string& valB) {
        if (!valA.empty() && !valB.empty() && toLower(valA) != toLower(valB)) {
            conflictFields.push_back(fieldName);
        }
    };
    
    checkField("First Name", a.firstName, b.firstName);
    checkField("Middle Name", a.middleName, b.middleName);
    checkField("Last Name", a.lastName, b.lastName);
    checkField("Company", a.company, b.company);
    checkField("Job Title", a.jobTitle, b.jobTitle);
    checkField("Birthday", a.birthday, b.birthday);
    checkField("Website", a.website, b.website);
    
    // Check home address (if both have one and they differ)
    if (!a.home_address.empty() && !b.home_address.empty() && 
        toLower(a.home_address) != toLower(b.home_address)) {
        conflictFields.push_back("Home Address");
    }
    
    // Check work address
    if (!a.work_address.empty() && !b.work_address.empty() && 
        toLower(a.work_address) != toLower(b.work_address)) {
        conflictFields.push_back("Work Address");
    }
    
    // Notes: both non-empty and different is a conflict (unless one contains the other)
    if (!a.notes.empty() && !b.notes.empty() && a.notes != b.notes) {
        // Check if one contains the other (then it's not really a conflict)
        if (a.notes.find(b.notes) == std::string::npos && b.notes.find(a.notes) == std::string::npos) {
            conflictFields.push_back("Notes");
        }
    }
    
    return !conflictFields.empty();
}

// Helper: Auto-merge source contact into target (no conflicts case)
void ContactMgr::autoMergeInto(Contact& target, const Contact& source) {
    // Merge phone numbers (add source's phones that target doesn't have)
    for (const auto& sPhone : source.phoneNumbers) {
        bool found = false;
        for (const auto& tPhone : target.phoneNumbers) {
            if (tPhone.number == sPhone.number) {
                found = true;
                break;
            }
        }
        if (!found) {
            target.phoneNumbers.push_back(sPhone);
        }
    }
    
    // Merge emails
    for (const auto& sEmail : source.emails) {
        bool found = false;
        for (const auto& tEmail : target.emails) {
            if (toLower(tEmail.address) == toLower(sEmail.address)) {
                found = true;
                break;
            }
        }
        if (!found) {
            target.emails.push_back(sEmail);
        }
    }
    
    // Fill in empty fields from source (pick longer if both exist for non-conflict)
    auto pickBest = [](const std::string& t, const std::string& s) {
        if (t.empty()) return s;
        if (s.empty()) return t;
        return (t.length() >= s.length()) ? t : s;
    };
    
    target.firstName = pickBest(target.firstName, source.firstName);
    target.middleName = pickBest(target.middleName, source.middleName);
    target.lastName = pickBest(target.lastName, source.lastName);
    target.company = pickBest(target.company, source.company);
    target.jobTitle = pickBest(target.jobTitle, source.jobTitle);
    target.birthday = pickBest(target.birthday, source.birthday);
    target.website = pickBest(target.website, source.website);
    
    // Merge notes
    if (target.notes.empty()) {
        target.notes = source.notes;
    } else if (!source.notes.empty() && target.notes != source.notes) {
        // Append if different
        if (target.notes.find(source.notes) == std::string::npos) {
            target.notes = target.notes + " | " + source.notes;
        }
    }
    
    // Addresses: fill in if target is empty
    if (target.home_address.empty() && !source.home_address.empty()) {
        target.home_address = source.home_address;
        target.home_city = source.home_city;
        target.home_state = source.home_state;
        target.home_zip = source.home_zip;
        target.home_country = source.home_country;
    }
    if (target.work_address.empty() && !source.work_address.empty()) {
        target.work_address = source.work_address;
        target.work_city = source.work_city;
        target.work_state = source.work_state;
        target.work_zip = source.work_zip;
        target.work_country = source.work_country;
    }
}

// Find potential duplicate contacts
std::vector<std::pair<int, int>> ContactMgr::findDuplicates() const {
    std::vector<std::pair<int, int>> duplicates;
    std::set<std::pair<int, int>> alreadyPaired;  // Avoid duplicate pairs
    
    // Only consider contacts with the same name as duplicates
    // (Skip phone/email matching since coworkers may share work numbers)
    for (const auto& [key, ids] : nameIndex) {
        // Skip empty name keys (contacts with no first or last name)
        if (key == "|" || key.empty()) continue;
        
        if (ids.size() > 1) {
            // Multiple contacts with the same name key
            for (size_t i = 0; i < ids.size(); ++i) {
                for (size_t j = i + 1; j < ids.size(); ++j) {
                    int id1 = std::min(ids[i], ids[j]);
                    int id2 = std::max(ids[i], ids[j]);
                    if (alreadyPaired.find({id1, id2}) == alreadyPaired.end()) {
                        duplicates.push_back({id1, id2});
                        alreadyPaired.insert({id1, id2});
                    }
                }
            }
        }
    }
    
    return duplicates;
}


// ============================================================================
// 17. Extract Names from Email-Only Contacts
// ============================================================================
// Extract names from email addresses for email-only contacts
// Also cleans up name fields that contain email addresses
size_t ContactMgr::extractNamesFromEmails() {
    size_t updatedCount = 0;
    
    for (auto& contact : contacts) {
        std::string emailSource;
        bool needsProcessing = false;
        
        // Case 1: firstName contains an email address (has @)
        if (contact.firstName.find('@') != std::string::npos) {
            emailSource = contact.firstName;
            needsProcessing = true;
        }
        // Case 2: lastName contains an email address (has @)
        else if (contact.lastName.find('@') != std::string::npos) {
            emailSource = contact.lastName;
            needsProcessing = true;
        }
        // Case 3: Original behavior - no first AND last name but have an email
        else if (contact.firstName.empty() && contact.lastName.empty() && !contact.emails.empty()) {
            emailSource = contact.emails[0].address;
            needsProcessing = !emailSource.empty();
        }
        
        if (!needsProcessing) continue;
        if (emailSource.empty()) continue;
        
        // Extract local part (before @)
        size_t atPos = emailSource.find('@');
        if (atPos == std::string::npos || atPos == 0) continue;
        
        // If the source was from a name field, ensure it's also in the emails list
        if (contact.firstName.find('@') != std::string::npos || 
            contact.lastName.find('@') != std::string::npos) {
            // Check if this email already exists in the contact's emails
            bool emailExists = false;
            std::string lowerEmail = toLower(emailSource);
            for (const auto& em : contact.emails) {
                if (toLower(em.address) == lowerEmail) {
                    emailExists = true;
                    break;
                }
            }
            // Add to emails if not already there
            if (!emailExists) {
                EmailAndType newEmail;
                newEmail.address = emailSource;
                newEmail.type = EmailType::Personal;
                contact.emails.push_back(newEmail);
                std::cout << "  Added to emails: " << emailSource << "\n";
            }
        }
        
        std::string localPart = emailSource.substr(0, atPos);
        std::string firstName, lastName;
        
        // Try to split on common separators: '.', '_', '-'
        size_t dotPos = localPart.find('.');
        size_t underPos = localPart.find('_');
        size_t dashPos = localPart.find('-');
        
        // Find the first separator
        size_t sepPos = std::string::npos;
        if (dotPos != std::string::npos) sepPos = dotPos;
        if (underPos != std::string::npos && (sepPos == std::string::npos || underPos < sepPos)) sepPos = underPos;
        if (dashPos != std::string::npos && (sepPos == std::string::npos || dashPos < sepPos)) sepPos = dashPos;
        
        if (sepPos != std::string::npos && sepPos > 0 && sepPos < localPart.length() - 1) {
            // Format: first.last or first_last or first-last
            firstName = capitalizeFirst(localPart.substr(0, sepPos));
            lastName = capitalizeFirst(localPart.substr(sepPos + 1));
            
            // Check if there's another separator (middle name case: first.middle.last)
            std::string remaining = localPart.substr(sepPos + 1);
            size_t nextSep = remaining.find_first_of("._-");
            if (nextSep != std::string::npos && nextSep > 0) {
                // Take last part as lastName
                lastName = capitalizeFirst(remaining.substr(nextSep + 1));
            }
        } else {
            // No separator - try to detect camelCase or just use as first name
            // Look for uppercase letter in middle (camelCase)
            size_t camelPos = std::string::npos;
            for (size_t i = 1; i < localPart.length(); ++i) {
                if (std::isupper(localPart[i])) {
                    camelPos = i;
                    break;
                }
            }
            
            if (camelPos != std::string::npos) {
                // CamelCase: johnDoe -> John, Doe
                firstName = capitalizeFirst(localPart.substr(0, camelPos));
                lastName = capitalizeFirst(localPart.substr(camelPos));
            } else {
                // Single word - use as first name, leave last name empty
                // Common pattern: jdoe -> could be initial + last name
                if (localPart.length() > 2 && localPart.length() <= 7) {
                    // Might be flast format (e.g., jsmith)
                    // Assume first char is initial, rest is last name
                    firstName = std::string(1, std::toupper(localPart[0]));
                    lastName = capitalizeFirst(localPart.substr(1));
                } else {
                    // Just use the whole thing as first name
                    firstName = capitalizeFirst(localPart);
                }
            }
        }
        
        // Apply the extracted names
        if (!firstName.empty() || !lastName.empty()) {
            std::string oldFirst = contact.firstName;
            std::string oldLast = contact.lastName;
            
            contact.firstName = firstName;
            contact.lastName = lastName;
            updatedCount++;
            
            std::cout << "Extracted: ";
            if (!oldFirst.empty() || !oldLast.empty()) {
                std::cout << "\"" << oldFirst << " " << oldLast << "\" -> ";
            } else {
                std::cout << emailSource << " -> ";
            }
            std::cout << firstName << " " << lastName << std::endl;
        }
    }
    
    // Rebuild indexes since names changed
    if (updatedCount > 0) {
        rebuildIndexes();
    }
    
    return updatedCount;
}

// ============================================================================
// 18.Bulk Operations
// ============================================================================

// Bulk update email domains from old format to new format
size_t ContactMgr::bulkUpdateEmailDomain(const std::string& oldDomain, const std::string& newDomain) {
    size_t updatedCount = 0;
    
    // Prepare the domain suffixes to match (case-insensitive)
    std::string oldSuffix = "@" + oldDomain;
    std::string newSuffix = "@" + newDomain;
    
    // Convert oldSuffix to lowercase for comparison
    std::string oldSuffixLower = oldSuffix;
    std::transform(oldSuffixLower.begin(), oldSuffixLower.end(), oldSuffixLower.begin(), ::tolower);
    
    for (auto& contact : contacts) {
        for (auto& emailEntry : contact.emails) {
            std::string email = emailEntry.address;
            if (email.empty()) continue;
            
            // Convert email to lowercase for comparison
            std::string emailLower = email;
            std::transform(emailLower.begin(), emailLower.end(), emailLower.begin(), ::tolower);
            
            // Check if email ends with old domain
            if (emailLower.length() >= oldSuffixLower.length()) {
                std::string emailSuffix = emailLower.substr(emailLower.length() - oldSuffixLower.length());
                
                if (emailSuffix == oldSuffixLower) {
                    // Replace the domain, preserving original case of local part
                    std::string localPart = email.substr(0, email.length() - oldSuffix.length());
                    std::string newEmail = localPart + newSuffix;
                    
                    std::cout << "Updated: " << email << " -> " << newEmail << std::endl;
                    emailEntry.address = newEmail;
                    updatedCount++;
                }
            }
        }
    }
    
    // Rebuild email index since addresses changed
    if (updatedCount > 0) {
        rebuildIndexes();
    }
    
    return updatedCount;
}


// ============================================================================
// 19. Renumber Contact IDs
// ============================================================================
void ContactMgr::renumberContactIds() {
    if (contacts.empty()) {
        std::cout << "No contacts to renumber." << std::endl;
        return;
    }
    
    std::cout << "\n=== Renumber Contact IDs ===\n";
    std::cout << "This will:\n";
    std::cout << "  1. Sort contacts alphabetically (by last name, then first name)\n";
    std::cout << "  2. Assign new sequential IDs starting from 1\n";
    std::cout << "  3. Save the changes\n\n";
    std::cout << "Current ID range: ";
    
    int minId = INT_MAX, maxId = 0;
    for (const auto& c : contacts) {
        if (c.contact_id < minId) minId = c.contact_id;
        if (c.contact_id > maxId) maxId = c.contact_id;
    }
    std::cout << minId << " to " << maxId << " (" << contacts.size() << " contacts)\n";
    std::cout << "New ID range will be: 1 to " << contacts.size() << "\n\n";
    
    std::cout << "Continue? (y/n): ";
    char confirm;
    std::cin >> confirm;
    
    if (std::tolower(confirm) != 'y') {
        std::cout << "Renumbering cancelled.\n";
        return;
    }
    
    // Sort contacts alphabetically
    std::sort(contacts.begin(), contacts.end(),
        [this](const Contact& a, const Contact& b) {
            std::string lastA = toLower(a.lastName);
            std::string lastB = toLower(b.lastName);
            if (lastA != lastB) return lastA < lastB;
            return toLower(a.firstName) < toLower(b.firstName);
        });
    
    // Assign new sequential IDs
    int newId = 1;
    for (auto& c : contacts) {
        c.contact_id = newId++;
    }
    
    // Rebuild indexes with new IDs
    rebuildIndexes();
    
    std::cout << "\nRenumbering complete!\n";
    std::cout << "Assigned IDs 1 through " << contacts.size() << "\n";
    std::cout << "Contacts are now sorted alphabetically.\n";
}



// ============================================================================
// 20. Save contacts to file (optimized - only writes non-empty fields)
// ============================================================================
bool ContactMgr::saveToFile(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }

    // Write header with version for future compatibility
    outFile << "#CONTACT_MGR_V2" << "\n";
    outFile << "#COUNT:" << contacts.size() << "\n";

    for (const auto& c : contacts) {
        outFile << "BEGIN_CONTACT" << "\n";
        outFile << "ID|" << c.contact_id << "\n";  // Always write ID
        
        // Only write non-empty fields
        if (!c.firstName.empty())   outFile << "FIRST|" << c.firstName << "\n";
        if (!c.middleName.empty())  outFile << "MIDDLE|" << c.middleName << "\n";
        if (!c.lastName.empty())    outFile << "LAST|" << c.lastName << "\n";
        if (!c.company.empty())     outFile << "COMPANY|" << c.company << "\n";
        if (!c.jobTitle.empty())    outFile << "JOBTITLE|" << c.jobTitle << "\n";
        
        // Emails - save each with type
        if (!c.emails.empty()) {
            for (const auto& email : c.emails) {
                outFile << "EMAIL|" << email.address << "|" << getEmailTypeStr(email.type) << "\n";
            }
        }
        
        // Phone numbers - only if there are any
        if (!c.phoneNumbers.empty()) {
            for (const auto& phone : c.phoneNumbers) {
                // Format: PHONE|number|type|extension (extension optional)
                outFile << "PHONE|" << phone.number << "|" << getPhoneTypeStr(phone.type);
                if (!phone.extension.empty()) {
                    outFile << "|" << phone.extension;
                }
                outFile << "\n";
            }
        }
        
        // Home address
        if (!c.home_address.empty()) outFile << "HOME_ADDR|" << c.home_address << "\n";
        if (!c.home_city.empty())    outFile << "HOME_CITY|" << c.home_city << "\n";
        if (!c.home_state.empty())   outFile << "HOME_STATE|" << c.home_state << "\n";
        if (!c.home_zip.empty())     outFile << "HOME_ZIP|" << c.home_zip << "\n";
        if (!c.home_country.empty()) outFile << "HOME_COUNTRY|" << c.home_country << "\n";
        
        // Work address
        if (!c.work_address.empty()) outFile << "WORK_ADDR|" << c.work_address << "\n";
        if (!c.work_city.empty())    outFile << "WORK_CITY|" << c.work_city << "\n";
        if (!c.work_state.empty())   outFile << "WORK_STATE|" << c.work_state << "\n";
        if (!c.work_zip.empty())     outFile << "WORK_ZIP|" << c.work_zip << "\n";
        if (!c.work_country.empty()) outFile << "WORK_COUNTRY|" << c.work_country << "\n";
        
        // Other address
        if (!c.other_street.empty())  outFile << "OTHER_STREET|" << c.other_street << "\n";
        if (!c.other_city.empty())    outFile << "OTHER_CITY|" << c.other_city << "\n";
        if (!c.other_state.empty())   outFile << "OTHER_STATE|" << c.other_state << "\n";
        if (!c.other_zip.empty())     outFile << "OTHER_ZIP|" << c.other_zip << "\n";
        if (!c.other_country.empty()) outFile << "OTHER_COUNTRY|" << c.other_country << "\n";
        
        // Other fields
        if (!c.birthday.empty())    outFile << "BIRTHDAY|" << c.birthday << "\n";
        if (!c.anniversary.empty()) outFile << "ANNIVERSARY|" << c.anniversary << "\n";
        if (!c.notes.empty())       outFile << "NOTES|" << c.notes << "\n";
        if (!c.website.empty())     outFile << "WEBSITE|" << c.website << "\n";
        
        outFile << "END_CONTACT" << "\n";
    }

    outFile.close();
    return true;
}


// Helper to capitalize first letter of a string
std::string ContactMgr::capitalizeFirst(const std::string& str) const {
    if (str.empty()) return str;
    std::string result = toLower(str);
    result[0] = std::toupper(result[0]);
    return result;
}


// Helper to parse phone type string
PhoneType ContactMgr::parsePhoneType(const std::string& typeStr) const {
    if (typeStr == "Home") return PhoneType::Home;
    if (typeStr == "Cell") return PhoneType::Cell;
    if (typeStr == "Work") return PhoneType::Work;
    if (typeStr == "School") return PhoneType::School;
    if (typeStr == "Icloud") return PhoneType::Icloud;
    return PhoneType::Cell; // Default
}


// Helper to parse email type string
EmailType ContactMgr::parseEmailType(const std::string& typeStr) const {
    if (typeStr == "Personal") return EmailType::Personal;
    if (typeStr == "Home") return EmailType::Home;
    if (typeStr == "Work") return EmailType::Work;
    if (typeStr == "School") return EmailType::School;
    if (typeStr == "Other") return EmailType::Other;
    return EmailType::Personal; // Default
}


// Load contacts from file
size_t ContactMgr::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        // File doesn't exist yet - not an error on first run
        return 0;
    }

    std::string line;
    Contact tempContact;
    bool inContact = false;
    size_t loadedCount = 0;
    int phoneCount = 0;

    while (std::getline(inFile, line)) {
        // Remove carriage return if present
        if (!line.empty() && line.back() == '\r') line.pop_back();
        
        if (line.empty() || line[0] == '#') continue; // Skip comments and empty lines

        if (line == "BEGIN_CONTACT") {
            inContact = true;
            tempContact = Contact();
            phoneCount = 0;
            continue;
        }

        if (line == "END_CONTACT") {
            if (inContact) {
                contacts.push_back(tempContact);
                loadedCount++;
                inContact = false;
            }
            continue;
        }

        if (inContact) {
            size_t delimPos = line.find('|');
            if (delimPos == std::string::npos) continue;

            std::string key = line.substr(0, delimPos);
            std::string value = line.substr(delimPos + 1);

            if (key == "ID") tempContact.contact_id = std::stoi(value);
            else if (key == "FIRST") tempContact.firstName = value;
            else if (key == "MIDDLE") tempContact.middleName = value;
            else if (key == "LAST") tempContact.lastName = value;
            else if (key == "COMPANY") tempContact.company = value;
            else if (key == "JOBTITLE") tempContact.jobTitle = value;
            else if (key == "EMAIL") {
                // Parse: EMAIL|address|type
                size_t secondDelim = value.find('|');
                if (secondDelim != std::string::npos) {
                    std::string address = value.substr(0, secondDelim);
                    std::string typeStr = value.substr(secondDelim + 1);
                    tempContact.emails.push_back({address, parseEmailType(typeStr)});
                } else {
                    // Old format without type - assume Personal
                    tempContact.emails.push_back({value, EmailType::Personal});
                }
            }
            else if (key == "PHONE_COUNT") phoneCount = std::stoi(value);
            else if (key == "PHONE") {
                // Parse: PHONE|number|type or PHONE|number|type|extension
                size_t secondDelim = value.find('|');
                if (secondDelim != std::string::npos) {
                    std::string number = value.substr(0, secondDelim);
                    std::string remaining = value.substr(secondDelim + 1);
                    
                    // Check for extension (third pipe)
                    size_t thirdDelim = remaining.find('|');
                    std::string typeStr;
                    std::string extension;
                    
                    if (thirdDelim != std::string::npos) {
                        typeStr = remaining.substr(0, thirdDelim);
                        extension = remaining.substr(thirdDelim + 1);
                    } else {
                        typeStr = remaining;
                    }
                    
                    tempContact.phoneNumbers.push_back({number, parsePhoneType(typeStr), extension});
                }
            }
            else if (key == "HOME_ADDR") tempContact.home_address = value;
            else if (key == "HOME_CITY") tempContact.home_city = value;
            else if (key == "HOME_STATE") tempContact.home_state = value;
            else if (key == "HOME_ZIP") tempContact.home_zip = value;
            else if (key == "HOME_COUNTRY") tempContact.home_country = value;
            else if (key == "WORK_ADDR") tempContact.work_address = value;
            else if (key == "WORK_CITY") tempContact.work_city = value;
            else if (key == "WORK_STATE") tempContact.work_state = value;
            else if (key == "WORK_ZIP") tempContact.work_zip = value;
            else if (key == "WORK_COUNTRY") tempContact.work_country = value;
            else if (key == "OTHER_STREET") tempContact.other_street = value;
            else if (key == "OTHER_CITY") tempContact.other_city = value;
            else if (key == "OTHER_STATE") tempContact.other_state = value;
            else if (key == "OTHER_ZIP") tempContact.other_zip = value;
            else if (key == "OTHER_COUNTRY") tempContact.other_country = value;
            else if (key == "BIRTHDAY") tempContact.birthday = value;
            else if (key == "ANNIVERSARY") tempContact.anniversary = value;
            else if (key == "NOTES") tempContact.notes = value;
            else if (key == "WEBSITE") tempContact.website = value;
        }
    }

    inFile.close();
    
    // Rebuild indexes after loading
    rebuildIndexes();
    
    return loadedCount;
}


// Add a single contact to all indexes
void ContactMgr::addToIndexes(const Contact& c) {
    // Name index
    std::string nameKey = makeNameKey(c.firstName, c.lastName);
    nameIndex[nameKey].push_back(c.contact_id);
    
    // Phone index - add all phone numbers
    for (const auto& phone : c.phoneNumbers) {
        if (!phone.number.empty()) {
            phoneIndex[phone.number] = c.contact_id;
        }
    }
    
    // Email index - add all emails
    for (const auto& email : c.emails) {
        if (!email.address.empty()) {
            emailIndex[toLower(email.address)] = c.contact_id;
        }
    }
}


// Rebuild all indexes from scratch (use after import/load)
void ContactMgr::rebuildIndexes() {
    // Clear existing indexes
    nameIndex.clear();
    phoneIndex.clear();
    emailIndex.clear();
    
    // Reserve space for efficiency
    size_t count = contacts.size();
    nameIndex.reserve(count);
    phoneIndex.reserve(count * 2);  // Assume avg 2 phones per contact
    emailIndex.reserve(count);
    
    // Rebuild
    for (const auto& c : contacts) {
        addToIndexes(c);
    }
}


// Check if a name already exists
bool ContactMgr::isDuplicateName(const std::string& firstName, const std::string& lastName) const {
    std::string key = makeNameKey(firstName, lastName);
    auto it = nameIndex.find(key);
    return (it != nameIndex.end() && !it->second.empty());
}


// Generate a unique name by appending (1), (2), etc.
std::string ContactMgr::makeUniqueName(const std::string& firstName, const std::string& lastName) const {
    int suffix = 1;
    std::string newFirst = firstName + "(" + std::to_string(suffix) + ")";
    
    while (isDuplicateName(newFirst, lastName)) {
        suffix++;
        newFirst = firstName + "(" + std::to_string(suffix) + ")";
    }
    
    return newFirst;
}


// Helper: Display a single field in a column
void printCol(const std::string& val, int width) {
    if (val.length() > static_cast<size_t>(width - 1)) {
        std::cout << val.substr(0, width - 4) << "...";
    } else {
        std::cout << std::left << std::setw(width) << val;
    }
}

// Display two contacts side by side for comparison
void ContactMgr::displaySideBySide(const Contact& left, const Contact& right) const {
    const int colWidth = 35;
    const int labelWidth = 14;
    
    std::cout << "\n" << std::string(85, '=') << "\n";
    std::cout << std::setw(labelWidth) << "" 
              << std::setw(colWidth) << "<< LEFT (L)" 
              << std::setw(colWidth) << "RIGHT (R) >>" << "\n";
    std::cout << std::string(85, '-') << "\n";
    
    // Lambda to print a row
    auto printRow = [&](const std::string& label, const std::string& l, const std::string& r, bool diff) {
        std::cout << std::setw(labelWidth) << std::left << label;
        if (diff && !l.empty() && !r.empty() && l != r) {
            std::cout << "\033[1;33m";  // Yellow for differences
        }
        printCol(l.empty() ? "(empty)" : l, colWidth);
        printCol(r.empty() ? "(empty)" : r, colWidth);
        if (diff && !l.empty() && !r.empty() && l != r) {
            std::cout << "\033[0m";  // Reset color
        }
        std::cout << "\n";
    };
    
    std::cout << std::setw(labelWidth) << "Contact ID:";
    printCol(std::to_string(left.contact_id), colWidth);
    printCol(std::to_string(right.contact_id), colWidth);
    std::cout << "\n";
    
    printRow("First Name:", left.firstName, right.firstName, true);
    printRow("Middle Name:", left.middleName, right.middleName, true);
    printRow("Last Name:", left.lastName, right.lastName, true);
    printRow("Company:", left.company, right.company, true);
    printRow("Job Title:", left.jobTitle, right.jobTitle, true);
    
    // Phone numbers
    size_t maxPhones = std::max(left.phoneNumbers.size(), right.phoneNumbers.size());
    for (size_t i = 0; i < maxPhones; ++i) {
        std::string lPhone = "", rPhone = "";
        if (i < left.phoneNumbers.size()) {
            lPhone = show_phone_mask(left.phoneNumbers[i].number) + " (" + 
                     getPhoneTypeStr(left.phoneNumbers[i].type) + ")";
            if (!left.phoneNumbers[i].extension.empty()) {
                lPhone += " x" + left.phoneNumbers[i].extension;
            }
        }
        if (i < right.phoneNumbers.size()) {
            rPhone = show_phone_mask(right.phoneNumbers[i].number) + " (" + 
                     getPhoneTypeStr(right.phoneNumbers[i].type) + ")";
            if (!right.phoneNumbers[i].extension.empty()) {
                rPhone += " x" + right.phoneNumbers[i].extension;
            }
        }
        std::string label = (i == 0) ? "Phone:" : "";
        printRow(label, lPhone, rPhone, false);
    }
    
    // Emails
    size_t maxEmails = std::max(left.emails.size(), right.emails.size());
    for (size_t i = 0; i < maxEmails; ++i) {
        std::string lEmail = "", rEmail = "";
        if (i < left.emails.size()) {
            lEmail = left.emails[i].address + " (" + getEmailTypeStr(left.emails[i].type) + ")";
        }
        if (i < right.emails.size()) {
            rEmail = right.emails[i].address + " (" + getEmailTypeStr(right.emails[i].type) + ")";
        }
        std::string label = (i == 0) ? "Email:" : "";
        printRow(label, lEmail, rEmail, false);
    }
    
    // Addresses
    if (!left.home_address.empty() || !right.home_address.empty()) {
        std::string lAddr = left.home_address + " " + left.home_city + " " + left.home_state;
        std::string rAddr = right.home_address + " " + right.home_city + " " + right.home_state;
        printRow("Home Addr:", lAddr, rAddr, true);
    }
    if (!left.work_address.empty() || !right.work_address.empty()) {
        std::string lAddr = left.work_address + " " + left.work_city + " " + left.work_state;
        std::string rAddr = right.work_address + " " + right.work_city + " " + right.work_state;
        printRow("Work Addr:", lAddr, rAddr, true);
    }
    
    printRow("Birthday:", left.birthday, right.birthday, true);
    printRow("Notes:", left.notes, right.notes, true);
    printRow("Website:", left.website, right.website, true);
    
    std::cout << std::string(85, '=') << "\n";
}

// Interactive merge with side-by-side comparison
// Helper: Read a single keypress (including arrow keys)
// Returns: 'L' for left arrow, 'R' for right arrow, 'U' for up, 'D' for down,
//          'S' for skip, 'Q' for quit, or the uppercase letter pressed
char readKey() {
#ifdef _WIN32
    // Windows implementation using conio.h
    int ch = _getch();
    
    // Check for special keys (arrow keys return 0 or 224 first)
    if (ch == 0 || ch == 224) {
        ch = _getch();  // Get the actual key code
        switch (ch) {
            case 75: return 'L';  // Left arrow
            case 77: return 'R';  // Right arrow
            case 72: return 'U';  // Up arrow
            case 80: return 'D';  // Down arrow
            default: return (char)std::toupper(ch);
        }
    }
    
    return (char)std::toupper(ch);
#else
    // Unix/Linux implementation using termios
    struct termios oldt, newt;
    char ch;
    
    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    
    // Disable canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    ch = getchar();
    
    // Check for escape sequence (arrow keys)
    if (ch == 27) {  // ESC
        char seq1 = getchar();
        if (seq1 == '[') {
            char seq2 = getchar();
            switch (seq2) {
                case 'D': ch = 'L'; break;  // Left arrow -> L
                case 'C': ch = 'R'; break;  // Right arrow -> R
                case 'A': ch = 'U'; break;  // Up arrow -> Skip
                case 'B': ch = 'D'; break;  // Down arrow -> reserved
            }
        }
    }
    
    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    return std::toupper(ch);
#endif
}

// Interactive merge with side-by-side comparison - with auto-merge for non-conflicts
size_t ContactMgr::interactiveMergeDuplicates() {
    auto duplicates = findDuplicates();
    
    if (duplicates.empty()) {
        std::cout << "No duplicate contacts found!" << std::endl;
        return 0;
    }
    
    // Count unique contacts involved in duplicates
    std::set<int> uniqueContactsInDups;
    for (const auto& [id1, id2] : duplicates) {
        uniqueContactsInDups.insert(id1);
        uniqueContactsInDups.insert(id2);
    }
    
    // Analyze duplicate reasons
    std::unordered_map<std::string, int> nameGroupCounts;
    nameGroupCounts.reserve(nameIndex.size());
    for (const auto& [key, ids] : nameIndex) {
        if (key != "|" && !key.empty() && ids.size() > 1) {
            nameGroupCounts[key] = ids.size();
        }
    }
    
    // First pass: count how many have conflicts vs can be auto-merged
    size_t conflictPairs = 0;
    size_t autoMergePairs = 0;
    for (const auto& [id1, id2] : duplicates) {
        const Contact* c1 = findContactById(id1);
        const Contact* c2 = findContactById(id2);
        if (c1 && c2) {
            std::vector<std::string> conflicts;
            if (hasConflicts(*c1, *c2, conflicts)) {
                conflictPairs++;
            } else {
                autoMergePairs++;
            }
        }
    }
    
    std::cout << "\n========== DUPLICATE ANALYSIS ==========\n";
    std::cout << "Total contacts: " << contacts.size() << "\n";
    std::cout << "Contacts involved in duplicates: " << uniqueContactsInDups.size() << "\n";
    std::cout << "Total duplicate pairs: " << duplicates.size() << "\n";
    std::cout << "\n\033[1;32mAuto-mergeable (no conflicts): " << autoMergePairs << "\033[0m\n";
    std::cout << "\033[1;33mNeed review (have conflicts): " << conflictPairs << "\033[0m\n";
    
    // Show top duplicate names
    if (!nameGroupCounts.empty()) {
        std::cout << "\nTop duplicate names:\n";
        std::vector<std::pair<std::string, int>> sortedNames(nameGroupCounts.begin(), nameGroupCounts.end());
        std::sort(sortedNames.begin(), sortedNames.end(), 
            [](const auto& a, const auto& b) { return a.second > b.second; });
        
        int shown = 0;
        for (const auto& [name, count] : sortedNames) {
            if (shown++ >= 5) break;
            std::cout << "  - \"" << name << "\": " << count << " contacts\n";
        }
    }
    
    std::cout << "\n=========================================\n";
    std::cout << "\nOptions:\n";
    std::cout << "  A = Auto-merge all non-conflicting pairs (" << autoMergePairs << " pairs)\n";
    std::cout << "  I = Interactive mode (review all pairs)\n";
    std::cout << "  C = Conflicts only (review only conflicting pairs)\n";
    std::cout << "  Q = Quit\n";
    std::cout << "\nChoice: ";
    
    char mode;
    std::cin >> mode;
    mode = std::toupper(mode);
    
    if (mode == 'Q') {
        std::cout << "Merge cancelled.\n";
        return 0;
    }
    
    bool autoMerge = (mode == 'A' || mode == 'C');
    bool conflictsOnly = (mode == 'C');
    
    if (autoMerge || conflictsOnly) {
        std::cout << "\n\033[1;36mControls for conflicts:\033[0m\n";
        std::cout << "  \033[1;33m<- Left Arrow\033[0m  = Keep LEFT contact\n";
        std::cout << "  \033[1;33m-> Right Arrow\033[0m = Keep RIGHT contact\n";
        std::cout << "  \033[1;33m^  Up Arrow\033[0m    = Skip this pair\n";
        std::cout << "  \033[1;33mQ\033[0m              = Quit\n\n";
    } else {
        std::cout << "\n\033[1;36mControls:\033[0m\n";
        std::cout << "  \033[1;33m<- Left Arrow\033[0m  = Keep LEFT contact\n";
        std::cout << "  \033[1;33m-> Right Arrow\033[0m = Keep RIGHT contact\n";
        std::cout << "  \033[1;33m^  Up Arrow\033[0m    = Skip this pair\n";
        std::cout << "  \033[1;33mQ\033[0m              = Quit\n\n";
    }
    
    std::cout << "Press Enter to start...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    
    size_t mergedCount = 0;
    size_t autoMergedCount = 0;
    size_t skippedCount = 0;
    size_t pairNum = 0;
    size_t conflictNum = 0;
    
    for (const auto& [id1, id2] : duplicates) {
        pairNum++;
        
        Contact* left = findContactByIdMutable(id1);
        Contact* right = findContactByIdMutable(id2);
        
        if (!left || !right) {
            continue;  // Already merged/deleted
        }
        
        std::vector<std::string> conflictFields;
        bool hasConflict = hasConflicts(*left, *right, conflictFields);
        
        // Auto-merge if no conflicts and auto-merge mode is on
        if (!hasConflict && autoMerge) {
            autoMergeInto(*left, *right);
            removeContactById(id2);
            autoMergedCount++;
            continue;
        }
        
        // Skip non-conflicts if conflicts-only mode
        if (!hasConflict && conflictsOnly) {
            continue;  // Already handled above, but just in case
        }
        
        if (conflictsOnly && !hasConflict) {
            continue;
        }
        
        conflictNum++;
        
        clearScreen();
        if (hasConflict) {
            std::cout << "\033[1;31m=== CONFLICT " << conflictNum << " of ~" << conflictPairs << " ===\033[0m\n";
            std::cout << "\033[1;33mConflicting fields: ";
            for (size_t i = 0; i < conflictFields.size(); ++i) {
                std::cout << conflictFields[i];
                if (i < conflictFields.size() - 1) std::cout << ", ";
            }
            std::cout << "\033[0m\n";
        } else {
            std::cout << "=== Pair " << pairNum << " of " << duplicates.size() << " ===\n";
        }
        
        displaySideBySide(*left, *right);
        
        std::cout << "\n[\033[1;33mL LEFT\033[0m] [\033[1;33mR RIGHT\033[0m] [\033[1;33mS SKIP\033[0m] [\033[1;33mQ QUIT\033[0m]: ";
        std::cout.flush();
        
        char choice = readKey();
        std::cout << "\n";
        
        if (choice == 'Q') {
            std::cout << "Quitting merge process.\n";
            break;
        }
        
        if (choice == 'S' || choice == 'U') {  // Skip or Up arrow
            skippedCount++;
            continue;
        }
        
        if (choice == 'L') {
            // Keep left, merge right into left
            autoMergeInto(*left, *right);
            removeContactById(id2);
            std::cout << "\033[1;32m* Kept LEFT\033[0m (merged into ID " << id1 << ")\n";
            mergedCount++;
        }
        else if (choice == 'R') {
            // Keep right, merge left into right
            autoMergeInto(*right, *left);
            removeContactById(id1);
            std::cout << "\033[1;32m* Kept RIGHT\033[0m (merged into ID " << id2 << ")\n";
            mergedCount++;
        }
    }
    
    // Rebuild indexes after all merges
    if (mergedCount > 0 || autoMergedCount > 0) {
        rebuildIndexes();
    }
    
    std::cout << "\n========== MERGE SUMMARY ==========\n";
    std::cout << "Auto-merged (no conflicts): " << autoMergedCount << "\n";
    std::cout << "Manually merged: " << mergedCount << "\n";
    std::cout << "Skipped: " << skippedCount << "\n";
    std::cout << "Total merged: " << (mergedCount + autoMergedCount) << "\n";
    std::cout << "===================================\n";
    
    return mergedCount + autoMergedCount;
}

// ============================================================================
// 13. Browse & Delete
// ============================================================================

// Interactive paginated browse with quick delete capability
size_t ContactMgr::interactiveBrowseDelete() {
    if (contacts.empty()) {
        std::cout << "No contacts to browse." << std::endl;
        return 0;
    }
    
    const int PAGE_SIZE = 10;
    
    // Create a sorted copy with original indices
    std::vector<std::pair<int, Contact>> sortedWithIds;
    sortedWithIds.reserve(contacts.size());
    for (const auto& c : contacts) {
        sortedWithIds.push_back({c.contact_id, c});
    }
    
    // Sort by last name, then first name
    std::sort(sortedWithIds.begin(), sortedWithIds.end(), 
        [this](const auto& a, const auto& b) {
            std::string lastA = toLower(a.second.lastName);
            std::string lastB = toLower(b.second.lastName);
            if (lastA != lastB) return lastA < lastB;
            return toLower(a.second.firstName) < toLower(b.second.firstName);
        });
    
    int totalPages = (sortedWithIds.size() + PAGE_SIZE - 1) / PAGE_SIZE;
    int currentPage = 0;
    std::set<int> markedForDeletion;  // Store contact_ids to delete
    size_t totalDeleted = 0;
    
    while (true) {
        clearScreen();
        
        int startIdx = currentPage * PAGE_SIZE;
        int endIdx = std::min(startIdx + PAGE_SIZE, static_cast<int>(sortedWithIds.size()));
        
        // Header
        std::cout << "\033[1;36m=== Browse & Delete Contacts ===";
        std::cout << " Page " << (currentPage + 1) << " of " << totalPages;
        std::cout << " (" << sortedWithIds.size() << " contacts) ===\033[0m\n";
        std::cout << std::string(90, '-') << "\n";
        
        // Display contacts on this page
        for (int i = startIdx; i < endIdx; ++i) {
            int displayNum = (i - startIdx) + 1;  // 1-10
            
            const auto& [id, contact] = sortedWithIds[i];
            bool isMarked = markedForDeletion.count(id) > 0;
            
            // Format: [X]  1. LastName, FirstName     Phone          Email
            std::cout << (isMarked ? "\033[1;31m[X]\033[0m " : "[ ] ");
            std::cout << std::right << std::setw(2) << displayNum << ". ";
            
            // Name (35 chars)
            std::string name = contact.lastName;
            if (!name.empty() && !contact.firstName.empty()) name += ", ";
            name += contact.firstName;
            if (name.empty()) name = "(no name)";
            if (name.length() > 33) name = name.substr(0, 30) + "...";
            std::cout << std::left << std::setw(35) << name;
            
            // Phone (20 chars)
            std::string phone = "(no phone)";
            if (!contact.phoneNumbers.empty()) {
                phone = show_phone_mask(contact.phoneNumbers[0].number);
            }
            if (phone.length() > 18) phone = phone.substr(0, 15) + "...";
            std::cout << std::setw(20) << phone;
            
            // Email (remaining)
            std::string email = "(no email)";
            if (!contact.emails.empty()) {
                email = contact.emails[0].address;
            }
            if (email.length() > 30) email = email.substr(0, 27) + "...";
            std::cout << email;
            
            std::cout << "\n";
        }
        
        std::cout << std::string(90, '-') << "\n";
        
        // Footer with controls
        std::cout << "\033[1;33mMarked for deletion: " << markedForDeletion.size() << "\033[0m\n";
        std::cout << "\n\033[1;36mControls:\033[0m\n";
        std::cout << "  [1-10] Toggle mark contact   |  [N]ext  [P]rev page\n";
        std::cout << "  [D] Delete marked            |  [Q]uit\n";
        std::cout << "  [< >] Prev/Next page         |  [J]ump to page\n";
        std::cout << "\nChoice: ";
        std::cout.flush();
        
        char key = readKey();
        std::cout << "\n";
        
        // Handle number keys (1-9) and special keys for 10
        if (key >= '1' && key <= '9') {
            int idx = startIdx + (key - '1');  // 1-9
            
            if (idx >= startIdx && idx < endIdx) {
                int contactId = sortedWithIds[idx].first;
                if (markedForDeletion.count(contactId)) {
                    markedForDeletion.erase(contactId);
                } else {
                    markedForDeletion.insert(contactId);
                }
            }
            continue;
        }
        
        // Handle '0' or '10' for the 10th item (pressing 1 then 0 won't work, so use 0)
        if (key == '0') {
            int idx = startIdx + 9;  // 10th item
            if (idx >= startIdx && idx < endIdx) {
                int contactId = sortedWithIds[idx].first;
                if (markedForDeletion.count(contactId)) {
                    markedForDeletion.erase(contactId);
                } else {
                    markedForDeletion.insert(contactId);
                }
            }
            continue;
        }
        
        switch (key) {
            case 'N':  // Next page
            case 'R':  // Right arrow
                if (currentPage < totalPages - 1) {
                    currentPage++;
                }
                break;
                
            case 'P':  // Previous page
            case 'L':  // Left arrow
                if (currentPage > 0) {
                    currentPage--;
                }
                break;
                
            case 'J':  // Jump to page
                {
                    std::cout << "Enter page number (1-" << totalPages << "): ";
                    int pageNum;
                    std::cin >> pageNum;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    if (pageNum >= 1 && pageNum <= totalPages) {
                        currentPage = pageNum - 1;
                    }
                }
                break;
                
            case 'D':  // Delete marked
                if (markedForDeletion.empty()) {
                    std::cout << "No contacts marked for deletion.\n";
                    std::cout << "Press any key to continue...";
                    readKey();
                } else {
                    std::cout << "\n\033[1;31mAbout to delete " << markedForDeletion.size() << " contact(s):\033[0m\n";
                    for (int id : markedForDeletion) {
                        const Contact* c = findContactById(id);
                        if (c) {
                            std::cout << "  - " << c->firstName << " " << c->lastName << "\n";
                        }
                    }
                    std::cout << "\n\033[1;33mConfirm delete? (Y/N): \033[0m";
                    std::cout.flush();
                    char confirm = readKey();
                    std::cout << "\n";
                    
                    if (confirm == 'Y') {
                        // Delete all marked contacts
                        for (int id : markedForDeletion) {
                            removeContactById(id);
                            totalDeleted++;
                        }
                        markedForDeletion.clear();
                        
                        // Rebuild sortedWithIds
                        sortedWithIds.clear();
                        for (const auto& c : contacts) {
                            sortedWithIds.push_back({c.contact_id, c});
                        }
                        std::sort(sortedWithIds.begin(), sortedWithIds.end(), 
                            [this](const auto& a, const auto& b) {
                                std::string lastA = toLower(a.second.lastName);
                                std::string lastB = toLower(b.second.lastName);
                                if (lastA != lastB) return lastA < lastB;
                                return toLower(a.second.firstName) < toLower(b.second.firstName);
                            });
                        
                        // Recalculate pages
                        totalPages = (sortedWithIds.size() + PAGE_SIZE - 1) / PAGE_SIZE;
                        if (totalPages == 0) totalPages = 1;
                        if (currentPage >= totalPages) currentPage = totalPages - 1;
                        
                        // Rebuild indexes
                        rebuildIndexes();
                        
                        std::cout << "\033[1;32mDeleted successfully!\033[0m Press any key...";
                        readKey();
                    } else {
                        std::cout << "Deletion cancelled. Press any key...";
                        readKey();
                    }
                }
                break;
                
            case 'Q':  // Quit
                if (!markedForDeletion.empty()) {
                    std::cout << "You have " << markedForDeletion.size() << " contact(s) marked.\n";
                    std::cout << "Quit without deleting? (Y/N): ";
                    std::cout.flush();
                    char confirm = readKey();
                    std::cout << "\n";
                    if (confirm != 'Y') {
                        continue;
                    }
                }
                std::cout << "\n=== Browse Summary ===";
                std::cout << "\nDeleted: " << totalDeleted << " contact(s)";
                std::cout << "\nRemaining: " << contacts.size() << " contact(s)\n";
                return totalDeleted;
                
            default:
                break;
        }
        
        // Check if we have any contacts left
        if (sortedWithIds.empty()) {
            std::cout << "All contacts deleted!\n";
            return totalDeleted;
        }
    }
}

// ============================================================================
// Utility Functions
// ============================================================================

// Function to clear the screen
void clearScreen() {
#ifdef _WIN32
    // Use ANSI escape codes - works better in VS Code terminal
    std::cout << "\033[2J\033[1;1H";
    std::cout.flush();
#else
    std::system("clear");
#endif
}

// Function to wait for user before repainting menu
void waitForEnter() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// ============================================================================
// Main Program
// ============================================================================

// Platform-dependent default paths
#ifdef _WIN32
    const std::string DEFAULT_SAVE_FILE = "C:\\Users\\tsciple\\.contact_mgr_data.txt";
    const std::string DEFAULT_DOWNLOADS = "C:\\Users\\tsciple\\Downloads\\";
#else
    const std::string DEFAULT_SAVE_FILE = "/home/ts/.contact_mgr_data.txt";
    const std::string DEFAULT_DOWNLOADS = "/home/ts/Downloads/";
#endif

int main(int argc, char const *argv[])
{

    ContactMgr cm;
    
    // Auto-load contacts on startup
    size_t loaded = cm.loadFromFile(DEFAULT_SAVE_FILE);
    if (loaded > 0) {
        std::cout << "Loaded " << loaded << " contacts from saved data." << std::endl;
    }
    
    int user_entry = 0;

    do
    {
        clearScreen();
        std::cout << "Contact Manager (" << cm.getContactCount() << " contacts loaded)\n" 
            << "============================================================\n"
            << " IMPORT                         | VIEW & SEARCH\n"
            << "  1. Import from VCF            |  6. View all contacts\n"
            << "  2. Import from Yahoo CSV      |  7. Search by name (exact)\n"
            << "                                |  8. Search by partial name\n"
            << " EXPORT                         |  9. Search by phone\n"
            << "  3. Export to CSV              | 10. Search by email\n"
            << "  4. Export to VCF              |-------------------------------\n"
            << "  5. Export to text             | ADD & EDIT\n"
            << "                                | 11. Add new contact\n"
            << " CLEANUP & MAINTENANCE          | 12. Edit contact\n"
            << " 14. Clean phone numbers        | 13. Browse & delete\n"
            << " 15. Remove dup phones          |-------------------------------\n"
            << " 16. Find & merge duplicates    | SYSTEM\n"
            << " 17. Extract names from emails  | 20. Save contacts\n"
            << " 18. Bulk update email domain   | 21. Exit\n"
            << " 19. Renumber contact IDs       |\n"
            << "============================================================\n"
            << "Choice: "; 

            if (!(std::cin >> user_entry)) {
                std::cout << "Invalid input. Please enter a number.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                waitForEnter();
                continue;
            }

        size_t number_imported =0;
        std::string full_path;
        switch (user_entry) {
            // --- IMPORT ---
            case 1:  // Import from VCF file
                number_imported = cm.importFromVCF(DEFAULT_DOWNLOADS + "All Contacts.vcf");
                std::cout << "Imported " << number_imported << " contacts from VCF." << std::endl;
                number_imported =0;
                waitForEnter();
                break;
            case 2:  // Import from Yahoo CSV
                number_imported = 0;
                full_path = "";
                std::cout << "Enter full path for import file or\n"
                          << "press Enter for default\n"
                          << "(" << DEFAULT_DOWNLOADS << "yahoo_contacts.csv): ";
                std::cin.ignore();  // Clear newline from menu input
                getline(std::cin, full_path);
                if (!full_path.empty()) {
                    number_imported=cm.importFromYahooCSV(full_path);
                } else {
                    number_imported=cm.importFromYahooCSV(DEFAULT_DOWNLOADS + "yahoo_contacts.csv");
                }
                std::cout << "Imported " << number_imported << " contacts from yahoo." << std::endl;
                waitForEnter();
                break;

            // --- EXPORT ---
            case 3:  // Export to pipe-delimited CSV
                std::cout << "Enter full path for export file or\n"
                          << "press Enter for default\n"
                          << "(" << DEFAULT_DOWNLOADS << "contacts_export.csv): ";
                std::cin.ignore();
                getline(std::cin, full_path);
                if (full_path.empty()) {
                    full_path = DEFAULT_DOWNLOADS + "contacts_export.csv";
                }
                cm.exportToPipeCSV(full_path);
                waitForEnter();
                break;
            case 4:  // Export to VCF (iPhone/iCloud)
                {
                    std::string vcfPath;
                    std::cout << "Enter full path for VCF export file\n"
                              << "(or press Enter for default " << DEFAULT_DOWNLOADS << "contacts_export.vcf): ";
                    std::cin.ignore();
                    std::getline(std::cin, vcfPath);
                    
                    if (vcfPath.empty()) {
                        vcfPath = DEFAULT_DOWNLOADS + "contacts_export.vcf";
                    }
                    
                    if (cm.exportToVCF(vcfPath)) {
                        std::cout << "\nVCF file created successfully!" << std::endl;
                        std::cout << "To import to iPhone:" << std::endl;
                        std::cout << "  1. Email the .vcf file to yourself" << std::endl;
                        std::cout << "  2. Open the email on your iPhone" << std::endl;
                        std::cout << "  3. Tap the attachment and select 'Add All Contacts'" << std::endl;
                        std::cout << "\nOr upload to iCloud.com -> Contacts -> Import vCard" << std::endl;
                    }
                }
                waitForEnter();
                break;
            case 5:  // Export to text file (printable)
                {
                    std::string txtPath;
                    std::cout << "Enter full path for text export file\n"
                              << "(or press Enter for default " << DEFAULT_DOWNLOADS << "contacts_export.txt): ";
                    std::cin.ignore();
                    std::getline(std::cin, txtPath);
                    
                    if (txtPath.empty()) {
                        txtPath = DEFAULT_DOWNLOADS + "contacts_export.txt";
                    }
                    
                    cm.exportToTextFile(txtPath);
                }
                waitForEnter();
                break;

            // --- VIEW & SEARCH ---
            case 6:  // View all contacts
                cm.viewAllContacts();
                waitForEnter();
                break;
            case 7:  // Search by name (exact)
                {
                    std::string searchName;
                    std::cout << "Enter name to search (Last,First or just Last): ";
                    std::cin.ignore();
                    std::getline(std::cin, searchName);
                    cm.searchName(searchName);
                }
                waitForEnter();
                break;
            case 8:  // Search by partial name
                {
                    std::string searchPartial;
                    std::cout << "Enter partial name to search: ";
                    std::cin.ignore();
                    std::getline(std::cin, searchPartial);
                    cm.searchNamePartial(searchPartial);
                }
                waitForEnter();
                break;
            case 9:  // Search by phone
                {
                    std::string searchPhone;
                    std::cout << "Enter phone number to search: ";
                    std::cin.ignore();
                    std::getline(std::cin, searchPhone);
                    cm.searchByPhone(searchPhone);
                }
                waitForEnter();
                break;
            case 10:  // Search by email
                {
                    std::string searchEmail;
                    std::cout << "Enter email to search: ";
                    std::cin.ignore();
                    std::getline(std::cin, searchEmail);
                    cm.searchByEmail(searchEmail);
                }
                waitForEnter();
                break;

            // --- ADD & EDIT ---
            case 11:  // Add new contact
                cm.addNewContact();
                waitForEnter();
                break;
            case 12:  // Edit contact
                {
                    std::string searchTerm;
                    std::cout << "Search for contact to edit (name, phone, or email): ";
                    std::cin.ignore();
                    std::getline(std::cin, searchTerm);
                    
                    // Try to find contact by different methods
                    std::cout << "\nSearching...\n";
                    cm.searchNamePartial(searchTerm);
                    
                    std::cout << "\nEnter contact ID to edit (or 0 to cancel): ";
                    int editId;
                    std::cin >> editId;
                    
                    if (editId > 0) {
                        cm.editContact(editId);
                    }
                }
                waitForEnter();
                break;
            case 13:  // Browse & delete contacts
                {
                    size_t deleted = cm.interactiveBrowseDelete();
                    if (deleted > 0) {
                        std::cout << "\nDeleted " << deleted << " contact(s)." << std::endl;
                    }
                }
                waitForEnter();
                break;

            // --- CLEANUP & MAINTENANCE ---
            case 14:  // Clean phone numbers
                cm.cleanAllPhoneNos();
                std::cout << "All phone numbers cleaned." << std::endl;
                waitForEnter();
                break;
            case 15:  // Remove duplicate phones within contacts
                {
                    size_t removed = cm.removeDuplicatePhones();
                    if (removed == 0) {
                        std::cout << "No duplicate phone numbers found within contacts." << std::endl;
                    }
                }
                waitForEnter();
                break;
            case 16:  // Find & merge duplicate contacts
                {
                    size_t merged = cm.interactiveMergeDuplicates();
                    if (merged > 0) {
                        std::cout << "\nMerged " << merged << " duplicate pair(s)." << std::endl;
                    }
                }
                waitForEnter();
                break;
            case 17:  // Extract names from email-only contacts
                {
                    size_t extracted = cm.extractNamesFromEmails();
                    std::cout << "\nExtracted names for " << extracted << " contact(s)." << std::endl;
                }
                waitForEnter();
                break;
            case 18:  // Bulk update email domain
                {
                    std::string oldDomain, newDomain;
                    std::cout << "Enter OLD email domain (e.g., rig-rds.com): ";
                    std::cin.ignore();
                    std::getline(std::cin, oldDomain);
                    std::cout << "Enter NEW email domain (e.g., richardepc.com): ";
                    std::getline(std::cin, newDomain);
                    
                    if (!oldDomain.empty() && !newDomain.empty()) {
                        size_t updated = cm.bulkUpdateEmailDomain(oldDomain, newDomain);
                        std::cout << "\nUpdated " << updated << " email(s) from @" << oldDomain 
                                  << " to @" << newDomain << std::endl;
                    } else {
                        std::cout << "Both old and new domains are required." << std::endl;
                    }
                }
                waitForEnter();
                break;
            case 19:  // Renumber contact IDs
                cm.renumberContactIds();
                waitForEnter();
                break;

            // --- SYSTEM ---
            case 20:  // Save contacts
                if (cm.saveToFile(DEFAULT_SAVE_FILE)) {
                    std::cout << "Contacts saved successfully to " << DEFAULT_SAVE_FILE << std::endl;
                } else {
                    std::cout << "Error saving contacts." << std::endl;
                }
                waitForEnter();
                break;
            case 21:  // Exit
                cm.saveToFile(DEFAULT_SAVE_FILE);
                std::cout << "Contacts saved. Exiting program." << std::endl;
                break;
            
            default:
                std::cout << "Invalid option.\n";
                waitForEnter();
                break;
        }

    } while (user_entry != 21);

    waitForEnter();
    return 0;
}