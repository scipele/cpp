# Contact Manager Upgrade Plan

## Overview
This document outlines the planned upgrades for the contact manager application to add data cleanup, deduplication, fast search, and iPhone/iCloud export capabilities.

---

## Progress Checklist

### Core Features
- [x] Auto-load contacts on open
- [x] Auto-save contacts on exit
- [x] Sparse field storage (only save non-empty fields)
- [x] Unordered maps for fast searches (name, phone, email indexes)
- [x] Search by phone number
- [x] Search by email address
- [x] Duplicate name detection on add (appends (1), (2), etc.)
- [x] Extract names from email-only contacts
- [x] Phone extension handling (separate extension field)
- [x] Email type support (Personal, Home, Work, School, Other)
- [x] Bulk update email domains (company rename)
- [x] Merge duplicate contacts (interactive side-by-side)
- [x] Remove duplicate phone numbers within a contact
- [x] VCF export for iPhone/iCloud
- [x] Text file export (printable format, same as View All)
- [x] Alphabetical sorting with letter separators (--A-- format)
- [x] Interactive browse & delete contacts (paginated)
- [x] Partial name search
- [x] Add new contact manually
- [x] Edit contact (add/edit phone, email, address, etc.)
- [x] Renumber contact IDs

### Menu Options Implemented (Current v2.0)
- [x] 1. Import from VCF file
- [x] 2. Import from Yahoo CSV file
- [x] 3. Export to pipe-delimited CSV
- [x] 4. Export to VCF (iPhone/iCloud)
- [x] 5. Export to text file (printable)
- [x] 6. View all contacts (sorted with --A-- separators)
- [x] 7. Search by name (exact)
- [x] 8. Search by partial name
- [x] 9. Search by phone
- [x] 10. Search by email
- [x] 11. Add new contact
- [x] 12. Edit contact
- [x] 13. Browse & delete contacts
- [x] 14. Clean phone numbers (remove formatting)
- [x] 15. Remove duplicate phones within contacts
- [x] 16. Find & merge duplicate contacts
- [x] 17. Extract names from email-only contacts
- [x] 18. Bulk update email domain
- [x] 19. Renumber contact IDs
- [x] 20. Save contacts
- [x] 21. Exit (with auto-save)

### Remaining Tasks
- [x] Partial name search (search any part of first/last name)
- [x] Manual contact editing (add/edit phone, email, address, etc.)
- [x] Renumber contact_ids alphabetically (cleanup/compact IDs)
- [x] Add new contact manually (not just import)

**All planned features have been implemented! 🎉**

---

## Feature List

### 1. Auto-Load Contacts on Open
**Difficulty:** Easy | **Priority:** High (do first)

- Add a `saveToFile()` method that serializes contacts to a binary or JSON file
- Add a `loadFromFile()` method that reads the saved contacts
- Call `loadFromFile()` at the start of `main()` before the menu loop
- Use a fixed path like `~/.contact_mgr/contacts.dat` or `contacts.json`

**New Methods:**
- `bool saveToFile(const std::string& filename)`
- `bool loadFromFile(const std::string& filename)`

---

### 2. Create Unordered Maps for Fast Searches
**Difficulty:** Medium | **Priority:** High

Add these private members to `ContactMgr`:
```cpp
std::unordered_map<std::string, std::vector<int>> nameIndex;      // lastName -> contact_ids
std::unordered_map<std::string, int> phoneIndex;                  // phone -> contact_id
std::unordered_map<std::string, int> emailIndex;                  // email -> contact_id
```

**Tasks:**
- Rebuild indexes after import or when contacts change
- Add `rebuildIndexes()` method
- Modify `searchName()` to use the index instead of linear scan
- Add `searchByPhone()` and `searchByEmail()` methods

**New Methods:**
- `void rebuildIndexes()`
- `void searchByPhone(const std::string& phone) const`
- `void searchByEmail(const std::string& email) const`

---

### 3. Merge Duplicate Contacts
**Difficulty:** Medium-Hard | **Priority:** Medium

Add `mergeDuplicates()` method with this logic:
1. Use `nameIndex` to find contacts with same last name
2. Compare first names (fuzzy match for "Bob" vs "Robert")
3. Compare phone numbers and emails for overlap
4. When duplicates found:
   - Keep the contact with more data as "master"
   - Merge phone numbers (union of both)
   - Merge emails, addresses, notes
   - Delete the duplicate
5. Add menu option to review merges interactively or auto-merge

**New Methods:**
- `std::vector<std::pair<int, int>> findDuplicates() const`
- `bool mergeContacts(int keepId, int removeId)`
- `size_t mergeDuplicates(bool interactive = true)`

---

### 4. Eliminate Duplicate Phone Numbers Within a Contact
**Difficulty:** Easy | **Priority:** Medium

Add `removeDuplicatePhones()` method:
- Compare normalized phone numbers (digits only)
- Keep the first occurrence, remove subsequent duplicates
- Can be called automatically during `cleanAllPhoneNos()`

**New Methods:**
- `size_t removeDuplicatePhones()`

**Implementation Notes:**
```cpp
// Use std::set to track unique phones per contact
std::set<std::string> seenPhones;
// Erase-remove idiom to remove duplicates
```

---

### 5. Clean Up Email-Only Contacts (Extract Name from Email)
**Difficulty:** Easy | **Priority:** Medium

Add `extractNamesFromEmails()` method:
1. Find contacts where `firstName` and `lastName` are empty but `email` is not
2. Parse email: `first.last@domain.com` → firstName="First", lastName="Last"
3. Handle variations: `firstlast@`, `first_last@`, `flast@`
4. Capitalize appropriately

**New Methods:**
- `size_t extractNamesFromEmails()`
- `std::string capitalizeFirst(const std::string& str)` (helper)

**Parsing Logic:**
```cpp
// Extract local part before @
// Split on '.', '_', or detect camelCase
// first.last@domain.com -> First, Last
// john_doe@domain.com -> John, Doe
```

---

### 6. Export for iPhone/iCloud Import
**Difficulty:** Medium | **Priority:** High

#### Option A: VCF Export (Recommended - Best for iPhone)
- vCard 3.0 format is universally supported
- Can be emailed to yourself and opened on iPhone
- Can be uploaded to iCloud.com → Contacts

**New Methods:**
- `bool exportToVCF(const std::string& filename)`

**VCF Format Example:**
```
BEGIN:VCARD
VERSION:3.0
N:Doe;John;;;
FN:John Doe
TEL;TYPE=CELL:+1 (555) 123-4567
TEL;TYPE=HOME:+1 (555) 987-6543
EMAIL:john.doe@email.com
END:VCARD
```

#### Option B: CSV for iCloud
- iCloud accepts CSV with specific column headers
- Headers: `First Name,Last Name,Phone 1 - Value,Phone 1 - Type,Email 1 - Value,...`

**New Methods:**
- `bool exportToiCloudCSV(const std::string& filename)`

---

### 7. Partial Name Search (TODO)
**Difficulty:** Easy-Medium | **Priority:** Medium

Search for contacts where any part of the first or last name matches the search term.

**Use Cases:**
- Search "john" finds "John Smith", "Johnson", "Johnny Appleseed", "Mary Johnson"
- Search "son" finds "Johnson", "Anderson", "Jason"

**New Methods:**
- `void searchNamePartial(const std::string& partial) const`

**Implementation Notes:**
```cpp
// Convert both search term and names to lowercase
// Use std::string::find() to check for substring match
// Return all contacts where firstName.find(partial) != npos OR lastName.find(partial) != npos
```

---

### 8. Manual Contact Editing (TODO)
**Difficulty:** Medium | **Priority:** High

Interactive menu to edit any contact field, add/remove phone numbers and emails.

**Features:**
- Search for contact first (by name, phone, or email)
- Display current contact info
- Present menu of editable fields
- Allow adding new phone numbers with type selection
- Allow adding new emails with type selection
- Allow editing/deleting existing phones and emails

**New Methods:**
- `bool editContact(int contact_id)`
- `void addPhoneToContact(Contact& c)`
- `void addEmailToContact(Contact& c)`
- `void editContactField(Contact& c, const std::string& fieldName)`

**Menu Structure:**
```
Editing: John Doe (ID: 42)
================================
1. First Name: John
2. Middle Name: (empty)
3. Last Name: Doe
4. Company: Acme Corp
5. Job Title: Engineer
---
6. Add phone number
7. Edit/delete phones (2 existing)
8. Add email
9. Edit/delete emails (1 existing)
---
10. Home address
11. Work address
---
S. Save changes
Q. Cancel/Quit
================================
```

---

### 9. Renumber Contact IDs (TODO)
**Difficulty:** Easy | **Priority:** Low

Compact and renumber all contact_ids sequentially, sorted alphabetically by last name, then first name.

**Why Needed:**
- After many merges/deletes, IDs can be sparse (1, 5, 23, 156, ...)
- Clean sequential IDs (1, 2, 3, 4, ...) are easier to work with
- Since contact_id is only used internally, renumbering is safe

**New Methods:**
- `void renumberContactIds()`

**Implementation Notes:**
```cpp
// 1. Sort contacts by lastName, then firstName (case-insensitive)
// 2. Assign new IDs starting from 1
// 3. Rebuild all indexes
// 4. Save to file
```

**Caution:**
- Should prompt for confirmation
- Should auto-save after renumbering

---

## Implementation Order

| Phase | Task | Status | New Methods |
|-------|------|--------|-------------|
| **1** | Auto-load/save | ✅ Done | `saveToFile()`, `loadFromFile()` |
| **2** | Build indexes | ✅ Done | `rebuildIndexes()`, `addToIndexes()` |
| **3** | Fast search | ✅ Done | `searchByPhone()`, `searchByEmail()` |
| **4** | Email name extraction | ✅ Done | `extractNamesFromEmails()` |
| **5** | Phone extensions | ✅ Done | Updated `PhoneNumberAndType` struct |
| **6** | Bulk email domain update | ✅ Done | `bulkUpdateEmailDomain()` |
| **7** | Remove dup phones | ✅ Done | `removeDuplicatePhones()` |
| **8** | Merge duplicates | ✅ Done | `findDuplicates()`, `interactiveMergeDuplicates()` |
| **9** | VCF export | ✅ Done | `exportToVCF()` |
| **10** | Text file export | ✅ Done | `exportToTextFile()` |
| **11** | Browse & delete | ✅ Done | `interactiveBrowseDelete()` |
| **12** | Partial name search | 🔲 TODO | `searchNamePartial()` |
| **13** | Manual contact edit | 🔲 TODO | `editContact()`, `addPhoneToContact()`, etc. |
| **14** | Renumber contact IDs | 🔲 TODO | `renumberContactIds()` |

---

## Updated Menu Structure

```
Contact Manager
=================================================
1.  Import from VCF file
2.  Import from Yahoo CSV file
3.  Clean phone numbers
4.  Remove duplicate phones within contacts  [NEW]
5.  Extract names from email-only contacts   [NEW]
6.  Find & merge duplicate contacts          [NEW]
7.  View all contacts
8.  Search by name (fast)
9.  Search by phone                          [NEW]
10. Search by email                          [NEW]
11. Export to VCF (iPhone/iCloud)            [NEW]
12. Export to pipe-delimited CSV
13. Save contacts
14. Exit
=================================================
```

---

## Additional Considerations

### Data Persistence Format Options
1. **JSON** - Human readable, easy to debug, larger file size
2. **Binary** - Smaller, faster, not human readable
3. **SQLite** - Robust, queryable, good for large datasets

**Recommendation:** Start with JSON for debugging, consider SQLite later.

### Error Handling
- Add validation for phone number formats
- Handle malformed email addresses gracefully
- Log merge operations for undo capability

### Future Enhancements
- [ ] Partial name search (search "john" finds "Johnson", "Johnny", etc.)
- [ ] Manual contact editing (interactive menu to edit any field)
- [ ] Renumber contact IDs alphabetically (compact and sequential)
- [ ] Add new contact manually (menu-driven entry)
- [ ] Google Contacts CSV import/export
- [ ] Contact groups/categories
- [ ] Undo last merge operation
- [ ] Backup before destructive operations
- [ ] Contact photo support (vCard PHOTO field)
- [ ] Import from Google Takeout format

---

## Dependencies to Add
```cpp
#include <set>           // For duplicate detection
#include <algorithm>     // For string transforms
#include <sstream>       // Already included
#include <regex>         // For email parsing (optional)
#include <filesystem>    // For file path handling (C++17)
```

---

## Notes
- Current file: `contact_mgr.cpp`
- Date created: January 18, 2026
- Last updated: January 18, 2026

---

## Changelog

| Date | Changes |
|------|---------|
| Jan 18, 2026 | Initial plan created |
| Jan 18, 2026 | Phase 1: Auto-load/save with sparse field storage |
| Jan 18, 2026 | Phase 2-3: Unordered maps, fast search by phone/email |
| Jan 18, 2026 | Added EmailType enum and EmailAndType struct |
| Jan 18, 2026 | Phase 4: Extract names from email-only contacts |
| Jan 18, 2026 | Added phone extension handling (extension field in struct) |
| Jan 18, 2026 | Added bulk email domain update feature |
| Jan 18, 2026 | Phase 8: Interactive merge duplicates (side-by-side comparison) |
| Jan 19, 2026 | Phase 7: Remove duplicate phones within contacts |
| Jan 19, 2026 | Phase 9: VCF export for iPhone/iCloud || Jan 19, 2026 | Phase 10: Text file export (same format as View All) |
| Jan 19, 2026 | Added alphabetical sorting with --A-- letter separators |
| Jan 19, 2026 | Phase 11: Interactive browse & delete (paginated, mark & delete) |
| Jan 19, 2026 | Added TODO: partial name search, manual edit, renumber IDs |