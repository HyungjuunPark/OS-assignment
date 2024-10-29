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

// TCHAR �迭�� string���� ��ȯ�ϴ� �Լ�
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

    // ù ��° ���μ��� ��������
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

// ���μ��� ������ ����ϴ� �Լ�
void printProcessTree(const vector<ProcessInfo>& processes) {
    // ���μ��� ID�� �̸��� �����ϴ� �� ����
    map<DWORD, string> processMap;
    for (const auto& p : processes) {
        processMap[p.processID] = p.processName;
    }

    cout << "################### Process Tree ######################" << '\n';
    cout << "Process Name\t\tProcess ID\tParent Process ID" << '\n';

    // ��� ���μ��� ���
    for (const auto& p : processes) {
        cout << p.processName << "\t"
            << p.processID << "\t\t"
            << p.parentProcessID << '\n'; // ���μ��� �̸�, ���μ��� ID, �θ� ���μ��� ID ������ ���
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
