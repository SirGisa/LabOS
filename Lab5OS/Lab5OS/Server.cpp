#include <windows.h>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    setlocale(LC_ALL, "RU");
    HANDLE hWritePipe1, hReadPipe1; // Pipe1: Server → Client
    HANDLE hWritePipe2, hReadPipe2; // Pipe2: Client → Server
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    // Pipe1
    CreatePipe(&hReadPipe1, &hWritePipe1, &sa, 0);
    // Pipe2
    CreatePipe(&hReadPipe2, &hWritePipe2, &sa, 0);

    // Ввод параметров
    long size;
    int N, M;
    cout << "Введите размер массива (long): ";
    cin >> size;
    cout << "Введите диапазон N и M (N < M): ";
    cin >> N >> M;

    // Структура передачи
    struct {
        long size;
        int N, M;
    } params = { size, N, M };

    // Подготовка запуска Client
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    TCHAR cmdLine[] = TEXT("Client.exe");

    // Унаследуемые дескрипторы
    SetHandleInformation(hWritePipe1, HANDLE_FLAG_INHERIT, 1);
    SetHandleInformation(hReadPipe2, HANDLE_FLAG_INHERIT, 1);

    bool i = CreateProcess(NULL, cmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

    if (!i) {
        cerr << "Ошибка запуска Client" << endl;
        return 1;
    }

    // Передаём параметры клиенту
    DWORD written;
    WriteFile(hWritePipe1, &params, sizeof(params), &written, NULL);

    // Чтение массива от клиента
    vector<long> data(size);
    DWORD read;
    ReadFile(hReadPipe2, data.data(), sizeof(long) * size, &read, NULL);

    cout << "Получен массив: ";
    for (long val : data)
        cout << val << " ";
    cout << endl;

    // Ожидание завершения
    cout << "Нажмите 'Q' для завершения..." << endl;
    while (toupper(cin.get()) != 'Q');

    CloseHandle(hReadPipe1);
    CloseHandle(hWritePipe1);
    CloseHandle(hReadPipe2);
    CloseHandle(hWritePipe2);
    return 0;
}
