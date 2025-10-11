//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | empty_recycle_bin.cpp                                       |
//| EntryPoint   | main                                                        |
//| Purpose      | fast cleanup recycle bin                                    |
//| Inputs       | none                                                        |
//| Outputs      | deleted files                                               |
//| Dependencies | standard only                                               |
//| By Name,Date | T.Sciple, 10/11/2025                                        |


#include <windows.h>
#include <shellapi.h>
#include <iostream> 


int main() {
    // Empty the Recycle Bin without confirmation dialog and no progress UI
    HRESULT result = SHEmptyRecycleBin(NULL, NULL, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
    
    if (SUCCEEDED(result)) {
        std::cout << "Recycle Bin emptied successfully." << std::endl;
    } else {
        std::cerr << "Failed to empty Recycle Bin. Error code: " << result << std::endl;
    }

    system("pause");
    
    return 0;
}