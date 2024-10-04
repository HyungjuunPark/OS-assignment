#include <iostream>
#include <windows.h>
#define ONE_SEC 1000
using namespace std;

// CPU Load ���
double GetCpuLoad() {
    FILETIME idleTime, kernelTime, userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime); 

    // FILETIME ����ü�� 64��Ʈ ������ ��ȯ�մϴ�.
    ULARGE_INTEGER idle, kernel, user;
    idle.LowPart = idleTime.dwLowDateTime;
    idle.HighPart = idleTime.dwHighDateTime;
    kernel.LowPart = kernelTime.dwLowDateTime;
    kernel.HighPart = kernelTime.dwHighDateTime;
    user.LowPart = userTime.dwLowDateTime;
    user.HighPart = userTime.dwHighDateTime;

    // ��ü CPU �ð� ���
    ULONGLONG totalKernelAndUser = kernel.QuadPart + user.QuadPart;
    ULONGLONG totalIdle = idle.QuadPart;

    // CPU �ε� ���
    double cpuLoad = 1.0 - (static_cast<double>(totalIdle) / static_cast<double>(totalKernelAndUser));

    return cpuLoad * 100.0; // �ۼ�Ʈ�� ��ȯ
}

// 1�ʸ��� ȭ�鿡 ����ð�, ���� CPU Load, 5/10/15�� ���� ��� CPU Load ���
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
        Sleep(ONE_SEC); // 1�� �������� ���
    }
    return 0;
}
