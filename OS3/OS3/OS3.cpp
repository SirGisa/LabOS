#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <ctime>

using namespace std;

// Глобальные объекты синхронизации
CRITICAL_SECTION cs;      
HANDLE          hMutex;   
HANDLE          hEvent;  

// Данные
vector<char>    arr;
vector<char>    processedArr;
int             k = 0;
double          avg = 0;
int             interval = 0;

// Поток work
DWORD WINAPI WorkThread(LPVOID)
{
    // Входим в кр. секцию
    EnterCriticalSection(&cs);
    cout << "[work] вход в кр. секцию" << endl;

    processedArr.assign(arr.size(), ' ');
    int pos = 0;
    for (char c : arr)
    {
        if (isdigit(static_cast<unsigned char>(c)))
        {
            processedArr[pos++] = c;
        }
        Sleep(interval);
    }

    cout << "[work] выход из кр. секции" << endl;
    LeaveCriticalSection(&cs);
    return 0;
}

// Поток SumElement
DWORD WINAPI SumElementThread(LPVOID)
{
    // Ожидаем сигнала от main (мьютекс)
    DWORD wait = WaitForSingleObject(hMutex, INFINITE);
    if (wait != WAIT_OBJECT_0)
    {
        cerr << "[SumElement] WaitForSingleObject failed: " << wait << endl;
        return 1;
    }

  //  EnterCriticalSection(&cs);
    cout << "[SumElement] вход в кр. секцию" << endl;

    int sum = 0, count = 0;
    for (int i = 0; i < (int)processedArr.size() && count < k; ++i)
    {
        char c = processedArr[i];
        if (isdigit(static_cast<unsigned char>(c)))
        {
            sum += c - '0';
            ++count;
        }
    }
    avg = (count > 0) ? double(sum) / count : 0.0;

    cout << "[SumElement] выход из кр. секции" << endl;
 //   LeaveCriticalSection(&cs);

    // Сигнал main, что готов результат
    if (!SetEvent(hEvent))
        cerr << "[SumElement] SetEvent failed: " << GetLastError() << endl;
    return 0;
}

int main()
{
    setlocale(LC_ALL, "RU");

    // Инициализация
    InitializeCriticalSection(&cs);

    hMutex = CreateMutex(NULL, TRUE, NULL);
    if (!hMutex)
    {
        cerr << "CreateMutex failed: " << GetLastError() << endl;
        return 1;
    }

    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!hEvent)
    {
        cerr << "CreateEvent failed: " << GetLastError() << endl;
        return 1;
    }

    // Ввод и валидация size
    int size = 0;
    cout << "Введите размер массива: ";
    if (!(cin >> size) || size <= 0)
    {
        cerr << "Неверный размер массива." << endl;
        return 1;
    }

    arr.resize(size);
    processedArr.resize(size);

    cout << "Введите элементы массива (символы): ";
    for (int i = 0; i < size; ++i)
        cin >> arr[i];

    cout << "Исходный массив: ";
    for (char c : arr) cout << c << ' ';
    cout << endl;

    // Ввод и коррекция интервала
    cout << "Введите интервал задержки (мс, >=0): ";
    cin >> interval;
    if (interval < 0) interval = 0;

    // Создаём потоки в подвешенном состоянии
    DWORD tidWork, tidSum;
    HANDLE hThreadWork = CreateThread(
        NULL, 0, WorkThread, NULL, CREATE_SUSPENDED, &tidWork
    );
    if (!hThreadWork)
    {
        cerr << "CreateThread(work) failed: " << GetLastError() << endl;
        return 1;
    }

    HANDLE hThreadSum = CreateThread(
        NULL, 0, SumElementThread, NULL, CREATE_SUSPENDED, &tidSum
    );
    if (!hThreadSum)
    {
        cerr << "CreateThread(SumElement) failed: " << GetLastError() << endl;
        return 1;
    }

    // Ввод и коррекция k
    cout << "Введите число k (<=" << size << "): ";
    cin >> k;
    if (k < 0) k = 0;
    if (k > size) k = size;

    // Запускаем work
    ResumeThread(hThreadWork);

    ResumeThread(hThreadSum);
    // Ждём его окончания
    Sleep(1);

    // main выводит первые k элементов итогового массива
    EnterCriticalSection(&cs);
    cout << "[main] вход в кр. секцию" << endl;
    cout << "Итоговый массив (первые k): ";
    for (int i = 0; i < k; ++i)
        cout << processedArr[i] << ' ';
    cout << endl;
    cout << "[main] выход из кр. секции" << endl;
    LeaveCriticalSection(&cs);

    // Отпускаем мьютекс, чтобы SumElement начал работу
    if (!ReleaseMutex(hMutex))
        cerr << "ReleaseMutex failed: " << GetLastError() << endl;
    

    // Ждём события от SumElement
    if (WaitForSingleObject(hEvent, INFINITE) != WAIT_OBJECT_0)
        cerr << "WaitForSingleObject(event) failed\n";

    cout << "Среднее арифметическое цифр (до k): " << avg << endl;

    if (WaitForSingleObject(hThreadWork, INFINITE) != WAIT_OBJECT_0)
        cerr << "WaitForSingleObject(work) failed\n";
    if (WaitForSingleObject(hThreadSum, INFINITE) != WAIT_OBJECT_0)
        cerr << "WaitForSingleObject(work) failed\n";
    // Очистка
    CloseHandle(hThreadWork);
    CloseHandle(hThreadSum);
    CloseHandle(hMutex);
    CloseHandle(hEvent);
    DeleteCriticalSection(&cs);

    return 0;
}
