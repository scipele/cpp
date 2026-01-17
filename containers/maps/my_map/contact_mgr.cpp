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


enum class PhoneType {
    Home,
    Cell,
    Work
};


struct PhoneNumberAndType {
    std::string number;
    PhoneType type;
};


struct Contact {
    std::string firstName;
    std::string lastName;
    std::vector<PhoneNumberAndType> phoneNumbers;
    std::string email;
};


class ContactMgr {
private:
    std::vector<Contact> contacts;

public:
    void add(const Contact& newContact);
    void viewAllContacts() const;
    void searchFirstName(const std::string& firstName) const;
    void importFromVCF(const std::string& filename);
    void cleanAllPhoneNos();
    void exportToPipeCSV(const std::string& filename); 
    std::string show_phone_mask(const std::string& str) const;
    void display(const Contact& cn) const;
    void out_pad(const std::string& strA,
                 const std::string& strB,
                 int wid) const;
};


// Class Inmplementation Functions:

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


void ContactMgr::display(const Contact& cn) const {
    if (!cn.firstName.empty()) out_pad("First_Name:", cn.firstName, 15);
    if (!cn.lastName.empty()) out_pad("Last_Name:", cn.lastName, 15);   
    
        // Inner loop: Iterate through every PhoneNumber struct in the contact's vector
        if (!cn.phoneNumbers.empty()) {
            std::cout << "Phone Numbers:"; 
            int8_t count = 0;
            std::string indent = " "; // Initial indent for the first number
            for (auto& pEntry : cn.phoneNumbers) {
                if (count++ > 0) indent = "               "; // Indent for subsequent numbers
                std::string type_str =  (pEntry.type == PhoneType::Home) ? "Home" :
                                        (pEntry.type == PhoneType::Cell) ? "Cell" :
                                        (pEntry.type == PhoneType::Work) ? "Work" : "Unknown";
                out_pad(indent + type_str, show_phone_mask(pEntry.number), indent.length() + 8);
            }
        }
        if (!cn.email.empty()) out_pad("Email: ", cn.email, 15);
        std::cout << "----------------------------------------------\n" << std::endl;
} 


// Method to search for a contact by first name
void ContactMgr::searchFirstName(const std::string& firstName) const {
    bool found = false;
    for (const auto& contact : contacts) {
        if (contact.firstName == firstName) {
            display(contact);
            found = true;
        }
    }
    if (!found) {
        std::cout << "No contact found with first name: " << firstName << std::endl;
    }
}


void ContactMgr::importFromVCF(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    Contact tempContact; // Use the name you declared
    bool inCard = false;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (line == "BEGIN:VCARD") {
            inCard = true;
            tempContact = Contact(); // Reset tempContact
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
    std::cout << "Import complete." << std::endl;
}


void ContactMgr::cleanAllPhoneNos() {
    // Outer loop: Iterate through every contact in the manager
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


int main(int argc, char const *argv[])
{
    ContactMgr cm;

    // Add some initial contacts
    cm.importFromVCF("/home/ts/Downloads/All Contacts.vcf");

    // Cleanup phone numbers
    cm.cleanAllPhoneNos();

    // View all contacts
    cm.viewAllContacts();

    cm.exportToPipeCSV("/home/ts/Downloads/contacts_export.csv");

    // Search for a specific contact
    cm.searchFirstName("John");
    cm.searchFirstName("Alice"); // Should show "Not found"
    return 0;
}
