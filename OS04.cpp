#include <iostream>
#include <vector>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <map>

using namespace std;

struct ProcessInfo {
    DWORD processID;
    string processName;
    DWORD parentProcessID;
};

// TCHAR 배열을 string으로 변환하는 함수
string ConvertTCharToString(const TCHAR* tCharStr) {
    char buffer[MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, tCharStr, -1, buffer, sizeof(buffer), NULL, NULL);
    return string(buffer);
}

vector<ProcessInfo> getProcessList() {
    vector<ProcessInfo> processes;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        cerr << "CreateToolhelp32Snapshot failed." << '\n';
        return processes;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // 첫 번째 프로세스 가져오기
    if (Process32First(hSnapshot, &pe32)) {
        do {
            ProcessInfo pInfo;
            pInfo.processID = pe32.th32ProcessID;
            pInfo.processName = ConvertTCharToString(pe32.szExeFile);
            pInfo.parentProcessID = pe32.th32ParentProcessID;

            processes.push_back(pInfo);
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    return processes;
}

// 프로세스 정보를 출력하는 함수
void printProcessTree(const vector<ProcessInfo>& processes) {
    // 프로세스 ID와 이름을 매핑하는 맵 생성
    map<DWORD, string> processMap;
    for (const auto& p : processes) {
        processMap[p.processID] = p.processName;
    }

    cout << "################### Process Tree ######################" << '\n';
    cout << "Process Name\t\tProcess ID\tParent Process ID" << '\n';

    // 모든 프로세스 출력
    for (const auto& p : processes) {
        cout << p.processName << "\t"
            << p.processID << "\t\t"
            << p.parentProcessID << '\n'; // 프로세스 이름, 프로세스 ID, 부모 프로세스 ID 순으로 출력
    }
}

int main() {
    vector<ProcessInfo> processes = getProcessList();

    if (processes.empty()) {
        cout << "No running processes found." << '\n';
    }
    else {
        cout << "Number of running processes: " << processes.size() << '\n';
        printProcessTree(processes);
    }

    return 0;
}
