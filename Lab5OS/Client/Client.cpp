#include <windows.h>
#include <iostream>
#include <vector>
#include <ctime>
using namespace std;

int main() {
    setlocale(LC_ALL, "RU");
    srand((unsigned)time(NULL));

    HANDLE hRead = GetStdHandle(STD_INPUT_HANDLE);   // Pipe1 read (from Server)
    HANDLE hWrite = GetStdHandle(STD_OUTPUT_HANDLE); // Pipe2 write (to Server)

    // Структура для чтения
    struct {
        long size;
        int N, M;
    } params;

    DWORD read;
    ReadFile(hRead, &params, sizeof(params), &read, NULL);

    // Генерация данных
    vector<long> data(params.size);
    for (long& x : data)
        x = rand() % (params.M - params.N + 1) + params.N;

    cout << "Сгенерированный массив: ";
    for (long val : data)
        cout << val << " ";
    cout << endl;

    DWORD written;
    WriteFile(hWrite, data.data(), sizeof(long) * params.size, &written, NULL);

    cout << "Нажмите 'Q' для завершения..." << endl;
    while (toupper(cin.get()) != 'Q');

    return 0;
}
