# Command Prompt
## Dir Command
-   dir "*hvy*" /s /o-d /t:w | findstr /r "^[0-9]"


# Powershell
## 1. Command to list all .xlsx files in a directory and its subdirectories, sorted by last modification date, and save the output to a text file:
- Get-ChildItem -Path "P:\2024\24-1467" -Recurse -File -Filter "*.xlsx" |
    Sort-Object LastWriteTime -Descending |
    Select-Object -ExpandProperty FullName |
    Out-File -FilePath "C:\t\file_list.txt" -Encoding UTF8

## 2. Write the parentpath|filename|last_mod_date to a text file
- "sep=| , parent_path|file_name|last_write_time" | Out-File -FilePath C:\t\file_list.csv -Encoding UTF8

Get-ChildItem -Path "T:\my_path" -Recurse -File -Filter *.xlsx | 
    Sort-Object LastWriteTime -Descending | 
    ForEach-Object { "$($_.DirectoryName)|$($_.Name)|$($_.LastWriteTime)" } | 
    Out-File -FilePath C:\t\file_list.csv -Encoding UTF8 -Append


