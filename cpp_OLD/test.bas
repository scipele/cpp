Option Explicit

'Uses Reference Library Microsoft Scripting Runtime to facilitate use of dictionary object early binding
Sub file_emails()
    Dim objNS As Outlook.NameSpace
    Dim folder_map_dict As Object
    Dim store_map_dict As Object
    Dim objStore As Outlook.store
    Dim objFolderRoot As Outlook.folder
    Dim est_proj_Folder As Outlook.folder
    Dim objItem As Object
    Dim key_fold As Variant
    Dim year_fold As String
    Dim year_subj As String
    Dim folder_name_str As String
    Dim key_subj As Variant
    Dim indx_store As Integer
    Dim indx_fld As Integer
    Dim target_folder As Variant
    Dim response As VbMsgBoxResult
    Dim folder_data As String
    Dim i As Long
    Dim count As Long
    Dim j As Long
    
    ' Initialize mapping array dictionary object that will be used to store keys for the folder names
    Set folder_map_dict = CreateObject("Scripting.Dictionary")
    Set store_map_dict = CreateObject("Scripting.Dictionary")
    
    ' Set up Outlook objects
    Set objNS = Application.GetNamespace("MAPI")
    
    ' Loop through all stores (PST files)
    For Each objStore In objNS.Stores
    indx_store = indx_store + 1   'start a counter to be able to save the index of the store
        
        ' Set the root folder to the default folder of the PST file
        Set objFolderRoot = objStore.GetRootFolder
        
        year_fold = get_year_from_root(objFolderRoot.Name)
        If year_fold <> "" Then
            
            ' Loop through each project or estimarte year folder within the year
            indx_fld = 0
            For Each est_proj_Folder In objFolderRoot.Folders
            indx_fld = indx_fld + 1   'start a counter to be able to save the index of the proj_folder
                ' Add key and folder name reference to the mapping array
                key_fold = get_key_ary(est_proj_Folder.Name)
                For i = LBound(key_fold) To UBound(key_fold) - 1
                    If key_fold(i) <> "0" Then
                        folder_map_dict(key_fold(i)) = year_fold & "|" & indx_fld & "|" & est_proj_Folder
                    End If
                Next i
    
            Next est_proj_Folder
            
            'setup a dictionary object to record the index number for each pst folder
            store_map_dict(year_fold) = indx_store
        End If
    Next objStore
    
    'Print out dictionary Values for reference
    Call print_dict(folder_map_dict)
    Call print_dict(store_map_dict)
    
    
    ' Loop through each inbox mail item in ** reverse order *** because objects are getting moved and it changes the index number and skips if you don't go backwards
    count = objNS.GetDefaultFolder(olFolderInbox).Items.count
    
    For i = count To 1 Step -1
        
        'prevent error where i is greater than object items count by setting i equal to the count
        If i > (objNS.GetDefaultFolder(olFolderInbox).Items.count) Then i = objNS.GetDefaultFolder(olFolderInbox).Items.count
        Set objItem = objNS.GetDefaultFolder(olFolderInbox).Items(i)
        If objItem.Class = olMail Then
            
            key_subj = get_key_ary(objItem.Subject)
            
            For j = LBound(key_subj) To UBound(key_subj)
                
                ' Check if email contains a key and is not flagged
                If key_subj(j) <> "0" And Not objItem.FlagStatus = 2 And Not objItem.UnRead Then
                
                    'set a variable to store the potential matching folder (0)=year, (1)=index, (2)=Full Folder Name
                    folder_data = folder_map_dict(key_subj(j))
                    year_subj = Mid(folder_data, 3, 2)
                    target_folder = get_folder_data(folder_data)
                    
                    If Not IsEmpty(target_folder) Then
                        'set the object store variable based on the recorded dictionary item
                        Set objStore = objNS.Stores(store_map_dict("20" & year_subj))
                        Set objFolderRoot = objStore.GetRootFolder
                        Set est_proj_Folder = objFolderRoot.Folders(target_folder(1))
        
                        ' Prompt the user to confirm the move
                        response = MsgBox("Do you want to move this email with subject " & vbNewLine & vbNewLine _
                        & objItem.Subject & vbNewLine & vbNewLine _
                        & " to folder " & vbNewLine & vbNewLine _
                        & target_folder(2) & "?", vbYesNo)
                        
                         ' If user selects Yes, move the email to the target folder
                        If response = vbYes Then
                            objItem.Move est_proj_Folder
                            i = i - 1
                            GoTo NextIterationA
                            
                        End If
                    End If
                End If
            Next j
            
        End If
        
NextIterationA:
    Next i
    
    
    ' Loop through each sent mail item in ** reverse order *** because objects are getting moved and it changes the index number and skips if you don't go backwards
    
    count = objNS.GetDefaultFolder(olFolderSentMail).Items.count
    
    For i = count To 1 Step -1
        Set objItem = objNS.GetDefaultFolder(olFolderSentMail).Items(i)
        If objItem.Class = olMail Then
            
            key_subj = get_key_ary(objItem.Subject)
            
            For j = LBound(key_subj) To UBound(key_subj)
                
                ' Check if email contains a key and is not flagged
                If key_subj(j) <> "0" And Not objItem.FlagStatus = 2 And Not objItem.UnRead Then
                
                    'set a variable to store the potential matching folder (0)=year, (1)=index, (2)=Full Folder Name
                    folder_data = folder_map_dict(key_subj(j))
                    year_subj = Mid(folder_data, 3, 2)
                    target_folder = get_folder_data(folder_data)
                    
                    If Not IsEmpty(target_folder) Then
                        'set the object store variable based on the recorded dictionary item
                        Set objStore = objNS.Stores(store_map_dict("20" & year_subj))
                        Set objFolderRoot = objStore.GetRootFolder
                        Set est_proj_Folder = objFolderRoot.Folders(target_folder(1))
        
                        ' Prompt the user to confirm the move
                        response = MsgBox("Do you want to move this email with subject " & vbNewLine & vbNewLine _
                        & objItem.Subject & vbNewLine & vbNewLine _
                        & " to folder " & vbNewLine & vbNewLine _
                        & target_folder(2) & "?", vbYesNo)
                        
                         ' If user selects Yes, move the email to the target folder
                        If response = vbYes Then
                            objItem.Move est_proj_Folder
                            i = i - 1
                            GoTo NextIterationA
                        End If
                    End If
                End If
            Next j
            
        End If
        
NextIterationB:
    Next i
    
    ' Clean up objects
    Set objNS = Nothing
    Set objStore = Nothing
    Set objFolderRoot = Nothing
    Set objItem = Nothing
    Set folder_map_dict = Nothing
End Sub



Function get_folder_data(ByVal folder_data As String)
    Dim fold As Variant
    
    If folder_data = Empty Then
        get_folder_data = Empty
    Else
        fold = Split(folder_data, "|")
        get_folder_data = fold
    End If
End Function



Function get_year_from_root(ByVal folder_name_str)
'Function returns the year format either (2022 -> 2022)
    Dim year As String
    If Len(folder_name_str) = 4 And IsNumeric(Mid(folder_name_str, 1, 4)) Then
        year = Mid(folder_name_str, 1, 4)
    End If
    
    get_year_from_root = Right(year, 4)
End Function


Public Function get_key_ary(ByVal str As String) As Variant
    Dim objRegExp As Object
    Dim objMatches As Object
    Dim matchText As Variant
    Dim i As Integer
 
    Set objRegExp = CreateObject("VBScript.RegExp")
    With objRegExp
        .Global = True
        .MultiLine = False
        .IgnoreCase = False
        ' Regex pattern used
        '23-4567    \d{2}-\d{4} matches a pattern of two digits followed by a hyphen and four digits.
        '2241234    \d{7} matches a pattern of seven digits.
        '241563     \d{6} matches a pattern of six digits.
        .Pattern = "\d{2}-\d{4}|\d{7}|\d{6}"
    End With
    
    Set objMatches = objRegExp.Execute(str)

    ' Check if any matches were found
    If objMatches.count > 0 Then
        ReDim matchText(0 To objMatches.count)
        For i = LBound(matchText) To UBound(matchText) - 1
                matchText(i) = objMatches.Item(i).Value
        Next i
        'set final
        matchText(objMatches.count) = "0"
    Else
        'go ahead and force it to have more than one dimension and indicate non match with "0" so that the function will return a consistent
        'variable type the the calling code to make it easier to handle
        ReDim matchText(0 To 1)
        matchText(0) = "0"
        matchText(1) = "0"
    End If

    ' Clean up
    Set objRegExp = Nothing

    get_key_ary = matchText
End Function



Public Function get_short_year(ByVal jb_est_no_str As String) As String
    '23-4567   -->  Return 23
    '2241234  -->  Return 24
    '241563   -->  Return 24
    Dim len_str As Integer
    
    len_str = Len(jb_est_no_str)
    
    get_short_year = Switch( _
                    len_str < 7, Left(jb_est_no_str, 2), _
                    InStr(1, jb_est_no_str, "-", vbTextCompare), Left(jb_est_no_str, 2), _
                    len_str = 7, Mid(jb_est_no_str, 2, 2), _
                    True, "")
End Function


Sub print_dict(ByVal dict As Object)
'This function prints the contents of a dictionary object for reference)
    
    Dim key As Variant
    For Each key In dict.Keys
        ' Print the key and corresponding item
        If key <> "" Then
            Debug.Print "key: " & key & " Item: "; dict(key)
        End If
    Next key
End Sub

