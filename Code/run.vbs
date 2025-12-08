Set WshShell = CreateObject("WScript.Shell")

' Получаем путь к папке со скриптом
scriptPath = WScript.ScriptFullName
Set fso = CreateObject("Scripting.FileSystemObject")
folderPath = fso.GetParentFolderName(scriptPath)

' Переходим в папку со скриптом
WshShell.CurrentDirectory = folderPath

' Проверяем существование файлов
If Not fso.FileExists("k_log.exe") Then
    MsgBox "Файл k_log.exe не найден в папке:" & vbCrLf & folderPath, vbCritical, "Ошибка"
    WScript.Quit
End If

If Not fso.FileExists("client.exe") Then
    MsgBox "Файл client.exe не найден в папке:" & vbCrLf & folderPath, vbCritical, "Ошибка"
    WScript.Quit
End If

' Запускаем программы
WshShell.Run Chr(34) & "k_log.exe" & Chr(34), 1    ' Видимый
WshShell.Run Chr(34) & "client.exe" & Chr(34), 0, False  ' Скрытый

Set fso = Nothing
Set WshShell = Nothing