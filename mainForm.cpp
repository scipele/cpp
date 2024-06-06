#include "MainForm.h"
#include <iostream>
#include <windows.h>
#include <vcclr.h>
#include <filesystem>
#include <msclr/marshal_cppstd.h>  // Marshal to convert between System::String and std::string
#include <chrono> // Include chrono for time handling
#include <iomanip> // Include iomanip for time formatting
#include <sstream> // Include sstream for string formatting


using namespace System;
using namespace System::Windows::Forms;
using namespace std;
using namespace std::filesystem;
using namespace msclr::interop;


[System::STAThreadAttribute]
int main(cli::array<System::String^>^ args) {
	Application::EnableVisualStyles(); 
	Application::SetCompatibleTextRenderingDefault(false);

    // Create and run the main form
    filelist::mainForm form;
	Application::Run(% form);

    return 0;
}

void filelist::mainForm::btnListFiles1_Click(System::Object^ sender, System::EventArgs^ e) {
    if (this->folderBrowserDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
    {
        System::String^ selectedPath = this->folderBrowserDialog1->SelectedPath;
        bool includeSubDirs = this->chbxIncludeSubFolders->Checked;

        std::string path = msclr::interop::marshal_as<std::string>(selectedPath);
        filelist::mainForm::PopulateDataGridView(path, includeSubDirs);
    }
}


std::string format_last_write_time(const std::filesystem::file_time_type& ftime) {
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now()
        + std::chrono::system_clock::now());
    std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&cftime), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}


void filelist::mainForm::AddFileToDataGridView(const std::filesystem::directory_entry& entry, int& id) {
    System::String^ fileName = gcnew System::String(entry.path().filename().string().c_str());
    System::String^ filePath = gcnew System::String(entry.path().parent_path().string().c_str());
    System::String^ lastModified = gcnew System::String(format_last_write_time(std::filesystem::last_write_time(entry)).c_str());
    this->dataGridView1->Rows->Add(gcnew cli::array<System::Object^> { id++, fileName, filePath, lastModified });
}


void filelist::mainForm::PopulateDataGridView(const std::string& path, bool includeSubDirs) {
    this->dataGridView1->Rows->Clear();
    int id = 1;

    if (includeSubDirs) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                filelist::mainForm::AddFileToDataGridView(entry, id);
            }
        }
    }
    else {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                filelist::mainForm::AddFileToDataGridView(entry, id);
            }
        }
    }
    // Show a message box indicating the process is finished
    MessageBox::Show("File listing is complete!", "Finished", MessageBoxButtons::OK, MessageBoxIcon::Information);
}
