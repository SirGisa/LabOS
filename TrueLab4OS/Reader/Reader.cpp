#include <windows.h>
#include <iostream>

int main() {
    setlocale(LC_ALL, "RU");

    // Открытие событий и семафора
    HANDLE hEventA = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventA");
    HANDLE hEventB = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventB");
    HANDLE hEventExit = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventExit");
    HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, L"ReaderSemaphore");

    if (!hEventA || !hEventB || !hEventExit || !hSemaphore) {
        std::cerr << "Ошибка открытия объектов синхронизации!" << std::endl;
        return 1;
    }

    HANDLE events[] = { hEventA, hEventB, hEventExit };

    while (true) {
        // 1. Захват семафора (ожидание активности)
        WaitForSingleObject(hSemaphore, INFINITE);
        std::cout << "Activ (захвачен семафор)" << std::endl;

        // 2. Ожидание сообщения или завершения
        DWORD eventIdx = WaitForMultipleObjects(3, events, FALSE, INFINITE) - WAIT_OBJECT_0;

        

        // 3. Обработка события
        switch (eventIdx) {
        case 0:  // "A"
            std::cout << "Получено сообщение: A" << std::endl;
            Sleep(100);
            break;
        case 1:  // "B"
            std::cout << "Получено сообщение: B" << std::endl;
            Sleep(100);
            break;
        case 2:  // Завершение
            std::cout << "Получен сигнал завершения" << std::endl;
            ReleaseSemaphore(hSemaphore, 1, NULL);
            CloseHandle(hEventA);
            CloseHandle(hEventB);
            CloseHandle(hEventExit);
            CloseHandle(hSemaphore);
            Sleep(5000);
            return 0;
        default:
            std::cerr << "Ошибка ожидания события!" << std::endl;
            return 1;
        }

        // 4. Освобождение семафора
        ReleaseSemaphore(hSemaphore, 1, NULL);
        std::cout << "Inactiv (семафор освобождён)" << std::endl;


    }


    return 0;
}