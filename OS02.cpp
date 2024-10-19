#include <iostream>
#include <windows.h>
#include <psapi.h>
#define ONE_SEC 1000
using namespace std;

// 컴퓨터의 현재 프로세스 수 계산
DWORD CountProcess() {
    DWORD processIDs[1024], cbNeeded;
    EnumProcesses(processIDs, sizeof(processIDs), &cbNeeded);
    DWORD processCount = cbNeeded / sizeof(DWORD);

    return processCount;  
}

// 1초마다 화면에 현재시간, 프로세스 수 출력
void Print_per_sec() {
    SYSTEMTIME st;

    while (true) {
        GetLocalTime(&st);
        DWORD processCount = CountProcess();
        cout << st.wYear << "."
            << st.wMonth << "."
            << st.wDay << " "
            << st.wHour << ":"
            << st.wMinute << ":"
            << st.wSecond << " : "
            << processCount << '\n';
        Sleep(ONE_SEC);
    }
}

int main() {
    Print_per_sec();
    return 0;
}
