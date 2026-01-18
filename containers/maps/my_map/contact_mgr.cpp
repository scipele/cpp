//| Item	     | Documentation Notes                                         |
//|--------------|-------------------------------------------------------------|
//| Filename     | example_unordered_map.cpp                                   |
//| EntryPoint   | main                                                        |
//| Purpose      | example showing how to use an unordered map                 |
//| Inputs       | n/a                                                         |
//| Outputs      | print examples to the screen                                |
//| Dependencies | see standard includes                                       |
//| By Name,Date | T.Sciple, 01/17/2025                                        |

#include <iostream>
#include <unordered_map>
#include <string>
#include <iomanip>
#include <vector>
#include <fstream>
#include <limits>

enum class PhoneType {
    Home,
    Cell,
    Work,
    School,
    Icloud
};


struct PhoneNumberAndType {
    std::string number;
    PhoneType type;
};


struct Contact {
    int contact_id;
    std::string firstName;
    std::string middleName;
    std::string lastName;
    std::string company;
    std::string jobTitle; 
    std::vector<PhoneNumberAndType> phoneNumbers;
    std::string email;
    std::string home_address, home_city, home_state, home_zip, home_country;
    std::string work_address, work_city, work_state, work_zip, work_country;
    std::string other_street, other_city, other_state, other_zip, other_country;
    std::string birthday;
    std::string anniversary;
    std::string notes;
    std::string website;
};


class ContactMgr {
private:
    std::vector<Contact> contacts;
    std::string getNextField(std::stringstream& ss);

public:
    void add(const Contact& newContact);
    void viewAllContacts() const;
    void searchName(const std::string& firstName) const;
    size_t importFromVCF(const std::string& filename);
    size_t importFromYahooCSV(const std::string& filename);
    bool cleanAllPhoneNos();
    void exportToPipeCSV(const std::string& filename); 
    std::string show_phone_mask(const std::string& str) const;
    void display(const Contact& cn) const;
    void out_pad(const std::string& strA,
                 const std::string& strB,
                 int wid) const;
    std::string getPhoneTypeStr(const PhoneType& type) const;
    int getNextId() const;
};


// Class Implementation Functions:

// Method to add a new contact
void ContactMgr::add(const Contact& newContact) {
    contacts.push_back(newContact);
    std::cout << "Contact added successfully." << std::endl;
}


// Method to view all contacts
void ContactMgr::viewAllContacts() const {
    if (contacts.empty()) {
        std::cout << "No contacts found." << std::endl;
        return;
    }
    std::cout << "\n--- All Contacts ---" << std::endl;
    for (const auto& contact : contacts) {
        display(contact);        
    }
    std::cout << "----------------------" << std::endl;
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
    std::cout << std::setw(wid) << std::left << strA << strB << "\n";
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

void ContactMgr::display(const Contact& cn) const {
    
    out_pad("contact_id:", std::to_string(cn.contact_id), 15);
    if (!cn.firstName.empty()) out_pad("First_Name:", cn.firstName, 15);
    if (!cn.lastName.empty()) out_pad("Last_Name:", cn.lastName, 15);   
    
        // Inner loop: Iterate through every PhoneNumber struct in the contact's vector
        if (!cn.phoneNumbers.empty()) {
            std::cout << "Phone Numbers:"; 
            int8_t count = 0;
            std::string indent = " "; // Initial indent for the first number
            for (auto& pEntry : cn.phoneNumbers) {
                if (count++ > 0) indent = "               "; // Indent for subsequent numbers
                std::string type_str = getPhoneTypeStr(pEntry.type);
                out_pad(indent + type_str, show_phone_mask(pEntry.number), indent.length() + 8);
            }
        }
        if (!cn.email.empty()) out_pad("Email: ", cn.email, 15);
        std::cout << "----------------------------------------------\n" << std::endl;
} 


// Method to search for a contact by first name
void ContactMgr::searchName(const std::string& last_comma_first_name) const {
    bool found = false;
    std::string first_name = last_comma_first_name.substr(last_comma_first_name.find(",") + 1);
    std::string last_name = last_comma_first_name.substr(last_comma_first_name.find(",") + 1);

    for (const auto& contact : contacts) {
        if (contact.firstName == first_name && contact.lastName == last_name) {
            display(contact);
            found = true;
        }
    }    
    if (!found) {
        std::cout << "No contact found with name: " << last_comma_first_name << std::endl;
    }
}


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
                    tempContact.phoneNumbers.push_back({{value},PhoneType::Home});
                } else if (key.find("TEL;type=CELL") == 0) {
                    tempContact.phoneNumbers.push_back({{value},PhoneType::Cell});
                } else if (key.find("TEL;type=WORK") == 0) {
                    tempContact.phoneNumbers.push_back({{value},PhoneType::Work});
                } else if (key.find("EMAIL") == 0) {
                    tempContact.email = value;
                }
            }
        }
    }
    file.close();
    return importedCount;
}


size_t ContactMgr::importFromYahooCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return -1;

    std::string line;
    // Skip the header row
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
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

        // 6-8: Emails
        std::string emailMain = getNextField(ss);
        std::string emailHome = getNextField(ss);
        std::string emailWork = getNextField(ss);
        
        // TODO: Map these to your Contact struct's email storage LATER
        c.email = emailMain; // For simplicity, just take the main email
        //if (!emailMain.empty()) c.emails.push_back({emailMain, EmailType::Other});
        //if (!emailHome.empty()) c.emails.push_back({emailHome, EmailType::Home});
        //if (!emailWork.empty()) c.emails.push_back({emailWork, EmailType::Work});
//
        // 9-15: Phone Numbers (Mapping Yahoo columns to your PhoneType)
        std::string pGen    = getNextField(ss); // General Phone
        std::string pHome   = getNextField(ss);
        std::string pWork   = getNextField(ss);
        std::string pPager  = getNextField(ss);
        std::string pFax    = getNextField(ss);
        std::string pMobile = getNextField(ss);
        std::string pOther  = getNextField(ss);

        if (!pHome.empty())   c.phoneNumbers.push_back({pHome,   PhoneType::Home});
        if (!pMobile.empty()) c.phoneNumbers.push_back({pMobile, PhoneType::Cell});
        if (!pWork.empty())   c.phoneNumbers.push_back({pWork,   PhoneType::Work});
        if (!pGen.empty())    c.phoneNumbers.push_back({pGen,    PhoneType::Cell});

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
    return true;
}

bool ContactMgr::cleanAllPhoneNos() {
    // Outer loop: Iterate through every contact in the manager
    if(contacts.empty()) return false;
    
    for (auto& contact : contacts) {
        
        // Inner loop: Iterate through every PhoneNumber struct in the contact's vector
        for (auto& pEntry : contact.phoneNumbers) {
            
            std::string cur_no = pEntry.number;
            std::string cleaned;

            // 1. Character-by-character cleaning
            for (char c : cur_no) {
                if (isdigit(c) || c == '+') {
                    cleaned += c;
                }
            }

            // 2. Formatting logic
            if (cleaned.length() == 10) {
                cleaned = "1" + cleaned; // Assume country code '1' if 10 digits
            }

            // 3. Save the cleaned string back to the specific phone entry
            pEntry.number = cleaned;
        }
    }
    return true;
}


void ContactMgr::exportToPipeCSV(const std::string& filename) {
std::ofstream outFile(filename);
if (!outFile.is_open()) {
    std::cerr << "Error: Could not create file " << filename << std::endl;
    return;
}

// Optional: Add 'sep=|' to help Excel automatically recognize the delimiter
outFile << "sep=|" << "\n";

// 1. Write the Header Row
outFile << "First Name|Last Name|Phone Number|Email" << "\n";

// 2. Iterate and write data
for (const auto& contact : contacts) {
    outFile << contact.firstName << "|"
            << contact.lastName << "|";

            // Iterate through phone numbers and write them separated by commas
    for (size_t i = 0; i < contact.phoneNumbers.size(); ++i) {
        outFile << contact.phoneNumbers[i].number;
        if (i < contact.phoneNumbers.size() - 1) {
            outFile << ","; // Separate multiple phone numbers with commas
        }
    }
           outFile << contact.email << "\n";
}

outFile.close();
std::cout << "Data successfully exported to " << filename << std::endl;
}


// Helper to safely extract CSV fields
std::string ContactMgr::getNextField(std::stringstream& ss) {
    std::string field;
    if (!std::getline(ss, field, ',')) return "";
    // Optional: Remove quotes if Yahoo exports them (e.g., "John")
    // if (!field.empty() && field.front() == '"') {
    //     field = field.substr(1, field.size() - 2);
    // }
    return field;
}


int ContactMgr::getNextId() const {
    int maxId = 0;
    for (const auto& c : contacts) {
        if (c.contact_id > maxId) {
            maxId = c.contact_id;
        }
    }
    return maxId + 1;
}


// Function to clear the screen
void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

// Function to wait for user before repainting menu
void pause() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int main(int argc, char const *argv[])
{

    ContactMgr cm;
    
    int user_entry = 0;

    do
    {
        clearScreen();
        std::cout << "Contact Manager\n" 
            << "=================================================\n" 
            << "Enter from the following options:\n"
            << "1. Import contacts from VCF file\n"
            << "2. Import contacts from Yahoo CSV file\n"
            << "3. Clean phone numbers, remove hard formatting\n"
            << "4. View all contacts\n"
            << "5. Export contacts to pipe-delimited CSV file\n"
            << "6. Search contact by first name\n"
            << "7. Exit\n"
            << "=================================================\n"
            << "Choice: "; 

            if (!(std::cin >> user_entry)) {
                std::cout << "Invalid input. Please enter a number.\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                pause();
                continue;
            }

        size_t number_imported =0;
        std::string full_path;
        switch (user_entry) {
            case 1:
                number_imported = cm.importFromVCF("/home/ts/Downloads/All Contacts.vcf");
                std::cout << "Imported " << number_imported << " contacts from VCF." << std::endl;
                number_imported =0;
                pause();
                break;
            case 2:
                number_imported = 0;
                number_imported= cm.importFromYahooCSV("/home/ts/Downloads/contacts.csv");
                std::cout << "Imported " << number_imported << " contacts from yahoo." << std::endl;
                pause();
                break;
            case 3:
                // Cleanup phone numbers
                cm.cleanAllPhoneNos();
                std::cout << "All phone numbers cleaned." << std::endl;
                pause();
                break;
            case 4:
                // View all contacts
                cm.viewAllContacts();
                pause();
                break;
            case 5:
                std::cout << "Enter full path for export file or\n"
                          << "press Enter for default\n"
                          << "/home/user/contacts_export.csv): ";
                getline(std::cin, full_path); // Clear newline from previous input
                if (!full_path.empty()) {
                    cm.exportToPipeCSV(full_path);
                } else {
                    cm.exportToPipeCSV("/home/ts/Downloads/contacts_export.csv");
                }
                // Export contacts to pipe-delimited CSV
                cm.exportToPipeCSV(full_path);
                pause();
                break;

            case 6:
                // Search for a specific contact
                cm.searchName("John");
                cm.searchName("Alice"); // Should show "Not found"
                pause();
                break;
            case 7:
                std::cout << "Exiting program." << std::endl;
                break;
            
            default:
                std::cout << "Invalid option.\n";
                pause();
                break;
        }

    } while (user_entry !=7);

    pause();
    return 0;
}
