#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

static LPCWSTR EVT_A = L"Global\\EvtA";
static LPCWSTR EVT_B = L"Global\\EvtB";
static LPCWSTR EVT_END = L"Global\\EvtEnd";
static LPCWSTR SEM_NAME = L"Global\\SemReaders";

int wmain()
{
    setlocale(LC_ALL, "RU");

    // 1) ���� M, N
    int M, N, msgCount;
    cout << "������� Reader-��������� (M)? ";  cin >> M;
    cout << "������� ������������ Active (N < M)? "; cin >> N;
    if (N <= 0 || N >= M) {
        cerr << "�������� N\n";
        return 1;
    }

    cout << "������� ��������� ���������? "; cin >> msgCount;

    // 2) ������ �������
    HANDLE hSem = CreateSemaphoreW(NULL, N, N, SEM_NAME);
    if (!hSem) {
        cerr << "CreateSemaphore failed: " << GetLastError() << endl;
        return 1;
    }

    // 3) ������ ������ �������
    HANDLE hEvtA = CreateEventW(NULL, TRUE, FALSE, EVT_A);
    HANDLE hEvtB = CreateEventW(NULL, TRUE, FALSE, EVT_B);
    HANDLE hEvtEnd = CreateEventW(NULL, TRUE, FALSE, EVT_END);
    if (!hEvtA || !hEvtB || !hEvtEnd) {
        cerr << "CreateEvent failed: " << GetLastError() << endl;
        return 1;
    }

    // 4) ��������� M Reader.exe
    vector<HANDLE> readers;
    readers.reserve(M);
    for (int i = 0; i < M; ++i) {
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        wchar_t cmd[64];
        wsprintfW(cmd, L"Reader.exe %d", i + 1);
        if (!CreateProcessW(NULL, cmd, NULL, NULL, FALSE,
            CREATE_NEW_CONSOLE, NULL, NULL,
            &si, &pi)) {
            cerr << "CreateProcess(Reader) failed: " << GetLastError() << endl;
            return 1;
        }
        CloseHandle(pi.hThread);
        readers.push_back(pi.hProcess);
    }

    // 5) ���������� msgCount ��� �A�/�B�
    for (int i = 0; i < msgCount; ++i) {
        char c;
        cout << "������� ��������� (A/B): ";
        cin >> c;
        if (c == 'A' || c == 'a') {
            SetEvent(hEvtA);
            Sleep(100);          // ��� ���� ����������
            ResetEvent(hEvtA);
        }
        else {
            SetEvent(hEvtB);
            Sleep(100);
            ResetEvent(hEvtB);
        }
    }

    // 6) ����� ������
    SetEvent(hEvtEnd);

    // ���, ���� ��� Reader.exe ��������
    WaitForMultipleObjects((DWORD)readers.size(), readers.data(), TRUE, INFINITE);

    // ���������
    for (HANDLE h : readers) CloseHandle(h);
    CloseHandle(hSem);
    CloseHandle(hEvtA);
    CloseHandle(hEvtB);
    CloseHandle(hEvtEnd);
    return 0;
}
