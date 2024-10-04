#include <iostream>
#include <windows.h>
#define ONE_SEC 1000
using namespace std;

// CPU Load 계산
double GetCpuLoad() {
    FILETIME idleTime, kernelTime, userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime); 

    // FILETIME 구조체는 64비트 값으로 변환합니다.
    ULARGE_INTEGER idle, kernel, user;
    idle.LowPart = idleTime.dwLowDateTime;
    idle.HighPart = idleTime.dwHighDateTime;
    kernel.LowPart = kernelTime.dwLowDateTime;
    kernel.HighPart = kernelTime.dwHighDateTime;
    user.LowPart = userTime.dwLowDateTime;
    user.HighPart = userTime.dwHighDateTime;

    // 전체 CPU 시간 계산
    ULONGLONG totalKernelAndUser = kernel.QuadPart + user.QuadPart;
    ULONGLONG totalIdle = idle.QuadPart;

    // CPU 로드 계산
    double cpuLoad = 1.0 - (static_cast<double>(totalIdle) / static_cast<double>(totalKernelAndUser));

    return cpuLoad * 100.0; // 퍼센트로 변환
}

// 1초마다 화면에 현재시간, 현재 CPU Load, 5/10/15초 동안 평균 CPU Load 출력
void Print_per_sec() {
    SYSTEMTIME st;

    GetLocalTime(&st);
    cout << st.wYear << "."
        << st.wMonth << "."
        << st.wDay << " "
        << st.wHour << ":"
        << st.wMinute << ":"
        << st.wSecond << " : ";
}

int main() {
    while (1) {
        Print_per_sec();
        double load = GetCpuLoad();
        if (load >= 0.0) {
            std::cout << load << "%\n";
        }
        Sleep(ONE_SEC); // 1초 간격으로 출력
    }
    return 0;
}
