#include <string> // Add this line to include std::string
#include <filesystem>

#pragma once


namespace filelist {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for mainForm
	/// </summary>
	public ref class mainForm : public System::Windows::Forms::Form
	{
	public:
		mainForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~mainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::FolderBrowserDialog^ folderBrowserDialog1;
	private: System::Windows::Forms::Button^ btnListFiles1;
	private: System::Windows::Forms::DataGridView^ dataGridView1;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ id;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ filename;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ filepath;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^ lastModifiedDate;
	private: System::Windows::Forms::CheckBox^ chbxIncludeSubFolders;
	private: System::Windows::Forms::Button^ btnClearFileList;










	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->btnListFiles1 = (gcnew System::Windows::Forms::Button());
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->id = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->filename = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->filepath = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->lastModifiedDate = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->chbxIncludeSubFolders = (gcnew System::Windows::Forms::CheckBox());
			this->btnClearFileList = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->BeginInit();
			this->SuspendLayout();
			// 
			// btnListFiles1
			// 
			this->btnListFiles1->Location = System::Drawing::Point(24, 12);
			this->btnListFiles1->Name = L"btnListFiles1";
			this->btnListFiles1->Size = System::Drawing::Size(152, 24);
			this->btnListFiles1->TabIndex = 0;
			this->btnListFiles1->Text = L"List Files 1";
			this->btnListFiles1->UseVisualStyleBackColor = true;
			this->btnListFiles1->Click += gcnew System::EventHandler(this, &mainForm::btnListFiles1_Click);
			// 
			// dataGridView1
			// 
			this->dataGridView1->AllowUserToOrderColumns = true;
			this->dataGridView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(4) {
				this->id, this->filename,
					this->filepath, this->lastModifiedDate
			});
			this->dataGridView1->Location = System::Drawing::Point(12, 39);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->Size = System::Drawing::Size(1674, 701);
			this->dataGridView1->TabIndex = 1;
			// 
			// id
			// 
			this->id->HeaderText = L"id";
			this->id->MinimumWidth = 40;
			this->id->Name = L"id";
			this->id->Width = 40;
			// 
			// filename
			// 
			this->filename->HeaderText = L"filename";
			this->filename->MinimumWidth = 500;
			this->filename->Name = L"filename";
			this->filename->Width = 500;
			// 
			// filepath
			// 
			this->filepath->HeaderText = L"filepath";
			this->filepath->MinimumWidth = 800;
			this->filepath->Name = L"filepath";
			this->filepath->Width = 800;
			// 
			// lastModifiedDate
			// 
			this->lastModifiedDate->HeaderText = L"lastModifiedDate";
			this->lastModifiedDate->MinimumWidth = 200;
			this->lastModifiedDate->Name = L"lastModifiedDate";
			this->lastModifiedDate->Width = 200;
			// 
			// chbxIncludeSubFolders
			// 
			this->chbxIncludeSubFolders->AutoSize = true;
			this->chbxIncludeSubFolders->Location = System::Drawing::Point(199, 16);
			this->chbxIncludeSubFolders->Name = L"chbxIncludeSubFolders";
			this->chbxIncludeSubFolders->Size = System::Drawing::Size(136, 17);
			this->chbxIncludeSubFolders->TabIndex = 2;
			this->chbxIncludeSubFolders->Text = L"List Files in Sub Folders";
			this->chbxIncludeSubFolders->UseVisualStyleBackColor = true;
			this->chbxIncludeSubFolders->CheckedChanged += gcnew System::EventHandler(this, &mainForm::chbxIncludeSubFolders_CheckedChanged);
			// 
			// btnClearFileList
			// 
			this->btnClearFileList->Location = System::Drawing::Point(361, 12);
			this->btnClearFileList->Name = L"btnClearFileList";
			this->btnClearFileList->Size = System::Drawing::Size(152, 24);
			this->btnClearFileList->TabIndex = 3;
			this->btnClearFileList->Text = L"Clear File List";
			this->btnClearFileList->UseVisualStyleBackColor = true;
			this->btnClearFileList->Click += gcnew System::EventHandler(this, &mainForm::btnClearFileList_Click);
			// 
			// mainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1705, 755);
			this->Controls->Add(this->btnClearFileList);
			this->Controls->Add(this->chbxIncludeSubFolders);
			this->Controls->Add(this->dataGridView1);
			this->Controls->Add(this->btnListFiles1);
			this->Name = L"mainForm";
			this->Text = L"List Files";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	public:
		System::Void btnListFiles1_Click(System::Object^ sender, System::EventArgs^ e);
		void AddFileToDataGridView(const std::filesystem::directory_entry& entry, int& id);
		void filelist::mainForm::PopulateDataGridView(const std::string& path, bool chbxIncludeSubFolders);
	private: System::Void chbxIncludeSubFolders_CheckedChanged(System::Object^ sender, System::EventArgs^ e) {
	}
	private: System::Void btnClearFileList_Click(System::Object^ sender, System::EventArgs^ e) {
		this->dataGridView1->Rows->Clear();
	}
};

};
