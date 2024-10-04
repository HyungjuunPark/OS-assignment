#include <iostream>
#include <windows.h>
#include <iomanip>
#include <vector>
#define ONE_SEC 1000
using namespace std;

// CPU Load 계산 함수
double CPU_Load() {
    static ULARGE_INTEGER prevIdleTime = { 0, 0 }, prevKernelTime = { 0, 0 }, prevUserTime = { 0, 0 };

    FILETIME idleTime, kernelTime, userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);

    // FILETIME 구조체를 ULARGE_INTEGER로 변환
    ULARGE_INTEGER idle, kernel, user;
    idle.LowPart = idleTime.dwLowDateTime;
    idle.HighPart = idleTime.dwHighDateTime;
    kernel.LowPart = kernelTime.dwLowDateTime;
    kernel.HighPart = kernelTime.dwHighDateTime;
    user.LowPart = userTime.dwLowDateTime;
    user.HighPart = userTime.dwHighDateTime;

    // 이전 시간과 현재 시간의 차이를 이용하여 CPU 로드 계산
    ULONGLONG idleDiff = idle.QuadPart - prevIdleTime.QuadPart;
    ULONGLONG kernelDiff = kernel.QuadPart - prevKernelTime.QuadPart;
    ULONGLONG userDiff = user.QuadPart - prevUserTime.QuadPart;
    ULONGLONG totalDiff = (kernelDiff + userDiff);
    ULONGLONG usageDiff = totalDiff - idleDiff;

    // 이전 값을 현재 값으로 업데이트
    prevIdleTime = idle;
    prevKernelTime = kernel;
    prevUserTime = user;

    double cpuLoad = static_cast<double>(usageDiff) / static_cast<double>(totalDiff) * 100.0;
    return cpuLoad;
}

// 벡터의 평균값 계산 함수
double CalculateAverage(const vector<double>& data) {
    if (data.empty()) return 0.0;
    double sum = 0;
    for (double value : data) {
        sum += value;
    }
    return sum / data.size();
}

// 1초마다 현재 CPU Load와 5, 10, 15초 평균을 출력
void Print_per_sec() {
    SYSTEMTIME st;
    vector<double> load5, load10, load15;

    int count = 0;

    while (1) {
        GetLocalTime(&st);
        cout << st.wYear << "."
            << st.wMonth << "."
            << st.wDay << " "
            << st.wHour << ":"
            << st.wMinute << ":"
            << st.wSecond << " : ";

        double load = CPU_Load();
        if (load >= 0.0) {
            cout << fixed << setprecision(2);
            cout <<"[CPU Load: " << load << "%] ";

            // 최근 5, 10, 15초의 CPU 로드를 저장
            load5.push_back(load);
            load10.push_back(load);
            load15.push_back(load);

            if (load5.size() > 5) {
                load5.erase(load5.begin()); // 초과된 값 제거
            }
            if (load10.size() > 10) {
                load10.erase(load10.begin()); // 초과된 값 제거
            }
            if (load15.size() > 15) {
                load15.erase(load15.begin()); // 초과된 값 제거
            }

            // 5, 10, 15초 평균 계산
            double avg5 = CalculateAverage(load5);
            double avg10 = CalculateAverage(load10);
            double avg15 = CalculateAverage(load15);

            // 5, 10, 15초 평균 출력
            if (count >= 5) cout << "[5sec avg: " << avg5 << "%] ";
            if (count >= 10) cout << "[10sec avg: " << avg10 << "%] ";
            if (count >= 15) cout << "[15sec avg: " << avg15 << "%] ";

            cout << '\n';
        }

        Sleep(ONE_SEC); // 1초 간격으로 출력
        count++;
    }
}

int main() {
    Print_per_sec();
    return 0;
}

