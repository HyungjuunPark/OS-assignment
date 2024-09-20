//EnumProcesses();
//GetLocalTime();
#include <iostream>
#include <windows.h>
using namespace std;

void CountProcess() {

}

void Print_per_sec() {

}
int main() {
    SYSTEMTIME st;
    GetLocalTime(&st);


    cout << st.wYear << "."
        << st.wMonth << "."
        << st.wDay << " "
        << st.wHour << ":"
        << st.wMinute << ":"
        << st.wSecond << " : " << '\n';
   
    return 0;
}
