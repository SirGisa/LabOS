#include <windows.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

struct ThreadData {
    vector<int> arr;
    int size;
};

DWORD WINAPI WorkerThread(LPVOID param) {
    ThreadData* data = (ThreadData*)param;
    
    int maxNegative = INT_MIN;
    
    for (int num : data->arr) {
        cout << num << " ";
        if (num < 0 && num > maxNegative) {
            maxNegative = num;
        }
    }
    cout << endl;
    if (maxNegative == INT_MIN) {
        cout <<endl<< "��� ������������� ��������� � �������." << endl;
    } else {
        cout <<endl << "������������ ������������� �������: " << maxNegative << endl;
    }

    return 0;
}

vector<int> GenerateArray(int size) {
    vector<int> arr(size);
    for (int i = 0; i < size; ++i) {
        arr[i] = rand() % 201 - 100; 
    }
    return arr;
}

int main() {
    srand(time(NULL));
    setlocale(LC_ALL, "RU");

    int size;
    cout << "������� ������ �������: " ;
    cin >> size;
    cout << endl;
    vector<int> arr = GenerateArray(size);

    ThreadData data = {arr, size};

    HANDLE hThread = CreateThread(
        NULL, 0, WorkerThread, &data, 0, NULL
    );

    SuspendThread(hThread);
    cout << "����� �������������" << endl;
    
    Sleep(100);

    cout << "����� �����������" << endl;
    ResumeThread(hThread);
    
    if (hThread == NULL) {
        cerr << "������ �������� ������!" << endl;
        return 1;
    }

    WaitForSingleObject(hThread, INFINITE);  

    CloseHandle(hThread);  

    return 0;
}
