#include <windows.h>
#include <iostream>
using namespace std;

static LPCWSTR EVT_A = L"Global\\EvtA";
static LPCWSTR EVT_B = L"Global\\EvtB";
static LPCWSTR EVT_END = L"Global\\EvtEnd";
static LPCWSTR SEM_NAME = L"Global\\SemReaders";

int wmain(int argc, wchar_t* argv[])
{
    setlocale(LC_ALL, "RU");
    int id = (argc > 1 ? _wtoi(argv[1]) : 0);

    // ��������� �������
    HANDLE hEvtA = OpenEventW(EVENT_ALL_ACCESS, FALSE, EVT_A);
    HANDLE hEvtB = OpenEventW(EVENT_ALL_ACCESS, FALSE, EVT_B);
    HANDLE hEvtEnd = OpenEventW(EVENT_ALL_ACCESS, FALSE, EVT_END);
    HANDLE hSem = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, FALSE, SEM_NAME);

    if (!hEvtA || !hEvtB || !hEvtEnd || !hSem) {
        cerr << "[" << id << "] OpenSync failed: " << GetLastError() << endl;
        return 1;
    }

    HANDLE handles[3] = { hEvtA, hEvtB, hEvtEnd };

    while (true) {
        DWORD idx = WaitForMultipleObjects(3, handles, FALSE, INFINITE);
        if (idx == WAIT_OBJECT_0 + 2) {
            // ��������� ������
            cout << "[" << id << "] End of session\n";
            break;
        }

        // ��������� A ��� B
        // ������� ��� ������� � ����� �� ������ N ������������
        WaitForSingleObject(hSem, INFINITE);
        cout << "[" << id << "] ��tiv: ������� ��������� "
            << (idx == WAIT_OBJECT_0 ? 'A' : 'B') << "\n";
        ReleaseSemaphore(hSem, 1, NULL);
        cout << "[" << id << "] Ina�tiv: ��������� �������\n";

        // --- ���, ���� ����� ������� ������ ��� ������� ---
        HANDLE hEvt = handles[idx];  // ���� hEvtA, ���� hEvtB
        // ���� ������� ������� ����������, ����
        while (WaitForSingleObject(hEvt, 0) == WAIT_OBJECT_0) {
            Sleep(5);
        }

    }

    // ������
    CloseHandle(hEvtA);
    CloseHandle(hEvtB);
    CloseHandle(hEvtEnd);
    CloseHandle(hSem);
    return 0;
}
