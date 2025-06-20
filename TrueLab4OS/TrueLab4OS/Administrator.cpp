#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

int main() {

    setlocale(LC_ALL, "RU");

    // 1. Инициализация объектов синхронизации
    HANDLE hEventA = CreateEvent(NULL, TRUE, FALSE, L"EventA");
    HANDLE hEventB = CreateEvent(NULL, TRUE, FALSE, L"EventB");
    HANDLE hEventExit = CreateEvent(NULL, TRUE, FALSE, L"EventExit");
    HANDLE hSemaphore = NULL;

    if (!hEventA || !hEventB || !hEventExit) {
        std::cerr << "Ошибка создания событий!" << std::endl;
        return 1;
    }

    // 2. Запрос количества процессов Reader (M)
    int M;
    std::cout << "Введите количество процессов Reader (M): ";
    std::cin >> M;

    // 3. Запрос количества активных Reader (N < M)
    int N;
    do {
        std::cout << "Введите количество активных процессов Reader (N < M): ";
        std::cin >> N;
    } while (N >= M);

    // Создание семафора
    hSemaphore = CreateSemaphore(NULL, N, N, L"ReaderSemaphore");
    if (!hSemaphore) {
        std::cerr << "Ошибка создания семафора!" << std::endl;
        return 1;
    }

    // 4. Запрос количества сообщений
    int msgCount;
    std::cout << "Введите количество сообщений: ";
    std::cin >> msgCount;

    // 5. Запуск процессов Reader
    std::vector<STARTUPINFO> si(M);
    std::vector<PROCESS_INFORMATION> pi(M);
    for (int i = 0; i < M; ++i) {
        ZeroMemory(&si[i], sizeof(STARTUPINFO));
        si[i].cb = sizeof(STARTUPINFO);
        ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));

        if (!CreateProcess(
            L"Reader.exe",   // Имя исполняемого файла Reader
            NULL,            // Командная строка
            NULL,            // Атрибуты процесса
            NULL,            // Потоки
            FALSE,           // Наследование дескрипторов
            CREATE_NEW_CONSOLE, // Каждый процесс в новом окне
            NULL,            // Окружение
            NULL,            // Текущая директория
            &si[i],          // STARTUPINFO
            &pi[i]           // PROCESS_INFORMATION
        )) {
            std::cerr << "Ошибка запуска процесса Reader!" << std::endl;
            return 1;
        }
    }

    // 6. Отправка сообщений ("A" или "B")
    for (int i = 0; i < msgCount; ++i) {
        std::string msg;
        std::cout << "Введите сообщение (A или B): ";
        std::cin >> msg;

        if (msg == "A") {
            SetEvent(hEventA);
            //Sleep(100);  
            ResetEvent(hEventA);
        }
        else if (msg == "B") {
            SetEvent(hEventB);
            //Sleep(100);
            ResetEvent(hEventB);
        }
        else {
            std::cout << "Некорректное сообщение!" << std::endl;
            i--;  // Пропуск неверного ввода
        }
    }

    // 7. Сигнал завершения
    SetEvent(hEventExit);
    Sleep(1000);  // Даём Reader'ам время завершиться

    // 8. Очистка
    CloseHandle(hEventA);
    CloseHandle(hEventB);
    CloseHandle(hEventExit);
    CloseHandle(hSemaphore);
    for (int i = 0; i < M; ++i) {
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }

    return 0;
}