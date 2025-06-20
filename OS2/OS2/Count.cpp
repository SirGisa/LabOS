#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

int main()
{
    setlocale(LC_ALL, "RU");

    unsigned long long a = 0, b = 1;
    cout << "Числа Фибоначчи: " << a << " " << b << " ";
    for (int i = 0; i < 10; ++i)
    {
        unsigned long long next = a + b;
        cout << next << " ";
        a = b;
        b = next;

        this_thread::sleep_for(chrono::milliseconds(500));
    }
    cout << endl;
    system("pause");
    return 0;
}
