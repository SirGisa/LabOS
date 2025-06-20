#include <windows.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>

using namespace std;

int main()
{
    setlocale(LC_ALL, "RU"); 

    int size;
    cout << "Введите размер массива: ";
    cin >> size;
    vector<char> arr(size);
    cout << "Введите элементы массива:" << endl;
    for (int i = 0; i < size; ++i)
    {
        cin >> arr[i];
    }


    ostringstream oss;
    oss << "Child.exe " << size;  
    for (int i = 0; i < size; ++i)
    {
        oss << " " << arr[i];
    }
    string cmdLineStr = oss.str();

    wstring wCmdLine(cmdLineStr.begin(), cmdLineStr.end());

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwYSize = 50;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    wchar_t commandLine[1024];
    wcscpy_s(commandLine, wCmdLine.c_str());

    BOOL result = CreateProcess(
        L"Child.exe",       
        commandLine,        
        NULL,               // Атрибуты безопасности процесса
        NULL,               // Атрибуты безопасности потока
        FALSE,              // Наследование дескрипторов
        CREATE_NEW_CONSOLE, // Флаги создания
        NULL,               // Среда нового процесса
        NULL,               // Текущий каталог
        &si,                // STARTUPINFO
        &pi                 // PROCESS_INFORMATION
    );
    if (!result)
    {
        cout << "Не удалось создать процесс Child. Ошибка: " << GetLastError() << endl;
        return 1;
    }


    WaitForSingleObject(pi.hProcess, INFINITE);

    // TerminateProcess(pi.hProcess, 0);

    STARTUPINFO siCount;
    ZeroMemory(&siCount, sizeof(siCount));
    siCount.cb = sizeof(siCount);
    PROCESS_INFORMATION piCount;
    ZeroMemory(&piCount, sizeof(piCount));

    result = CreateProcess(
        L"Count.exe",       
        NULL,               
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &siCount,
        &piCount
    );
    if (!result)
    {
        cout << "Не удалось создать процесс Count. Ошибка: " << GetLastError() << endl;
        return 1;
    }

    SetPriorityClass(piCount.hProcess, HIGH_PRIORITY_CLASS);

    WaitForSingleObject(piCount.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(piCount.hProcess);
    CloseHandle(piCount.hThread);

    ExitProcess(0);

    return 0;
}
