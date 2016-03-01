'---------------------------------------------------------------------------------------
' Example for how to invoke the port scanner dll
'---------------------------------------------------------------------------------------
' This is a VBA script that can be used as an Microsoft Office macro 
' which loads the dll into the address space of the host program (ex. Excel, Word, etc)
'---------------------------------------------------------------------------------------

'Loads a library into the current process
Private Declare Function LoadLibrary Lib "kernel32" Alias "LoadLibraryA" (ByVal lpLibFileName As String) As Long

'Check if the file exists
Public Function FileExists(sFile$) As Boolean
    If Trim(sFile) = vbNullString Then Exit Function
    
        FileExists = IIf(Dir(sFile, vbArchive + vbHidden + vbReadOnly + vbSystem) <> vbNullString, True, False)
End Function

Sub portscan()
    sLibrary$ = "portscan.dll"
    
    If Not FileExists(sLibrary) Then
        MsgBox "DLL file does not exist."
        Exit Sub
    End If
    
    If LoadLibrary(sLibrary) = 0 Then
        MsgBox "LoadLibrary returned NULL"
    End If
End Sub
