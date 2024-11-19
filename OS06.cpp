#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <iomanip>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

// 프로세스 구조체 정의
struct Process {
    string id;
    int arrival_time;
    queue<int> cpu_bursts;
    queue<int> io_bursts;
    int waiting_time = 0;  // 누적 대기 시간
    int remaining_time = 0; // 현재 CPU burst 남은 시간
};

// SJF 우선순위 비교 (CPU burst 기준)
struct Compare {
    bool operator()(const Process* a, const Process* b) {
        return a->remaining_time > b->remaining_time; // 짧은 burst가 우선
    }
};

// 파일 입력 처리
vector<Process> get_processes_from_file(const string& filepath) {
    vector<Process> processes;
    ifstream input_file(filepath);

    if (!input_file.is_open()) {
        cerr << "Error: Unable to open file " << filepath << endl;
        exit(EXIT_FAILURE);
    }

    // 파일 내용을 출력
    string line;
    while (getline(input_file, line)) {
        if (line.empty()) continue;
        cout << line << endl; // 파일 내용 그대로 출력

        // 프로세스 정보 파싱
        stringstream ss(line);
        Process p;
        string token;

        ss >> p.id; // 프로세스 이름 읽기

        // 도착 시간 및 CPU/I/O 버스트 읽기
        while (ss >> token) {
            if (token.find("ARRIVAL:") != string::npos) {
                p.arrival_time = stoi(token.substr(token.find(":") + 1));
            }
            else if (token.find("CPU:") != string::npos) {
                p.cpu_bursts.push(stoi(token.substr(token.find(":") + 1)));
            }
            else if (token.find("I/O:") != string::npos) {
                p.io_bursts.push(stoi(token.substr(token.find(":") + 1)));
            }
        }
        processes.push_back(p);
    }

    input_file.close();
    return processes;
}

// SJF 스케줄링 시뮬레이션
void sjf_scheduling(const vector<Process>& processes) {
    auto ready_queue = priority_queue<Process*, vector<Process*>, Compare>();
    vector<Process> process_list = processes;
    map<string, int> waiting_times; // 프로세스 대기 시간 기록
    int current_time = 0; // 현재 시각
    int completed = 0;    // 완료된 프로세스 수
    int total_waiting_time = 0;

    while (completed < process_list.size()) {
        // 새로 Ready 상태로 들어오는 프로세스 추가
        for (auto& p : process_list) {
            if (p.arrival_time <= current_time && p.remaining_time == 0 && !p.cpu_bursts.empty()) {
                p.remaining_time = p.cpu_bursts.front(); // 현재 CPU burst 가져오기
                ready_queue.push(&p);
            }
        }

        // CPU 처리
        if (!ready_queue.empty()) {
            Process* current_process = ready_queue.top();
            ready_queue.pop();

            current_process->cpu_bursts.pop(); // 현재 CPU burst 완료
            int burst_time = current_process->remaining_time;
            current_time += burst_time; // 현재 시간을 진행
            current_process->remaining_time = 0;

            // I/O 준비가 되어 있으면 처리
            if (!current_process->io_bursts.empty()) {
                current_process->arrival_time = current_time + current_process->io_bursts.front();
                current_process->io_bursts.pop();
            }
            else {
                // 모든 CPU burst가 끝났다면 프로세스 종료
                completed++;
                waiting_times[current_process->id] = current_process->waiting_time;
            }

            // 다른 프로세스 대기 시간 갱신
            for (auto& p : process_list) {
                if (p.remaining_time == 0 && p.arrival_time <= current_time && &p != current_process) {
                    p.waiting_time += burst_time;
                }
            }
        }
        else {
            // 실행 가능한 프로세스가 없으면 시간 증가
            current_time++;
        }
    }

    // 결과 출력
    cout << "\nProcess\t: Waiting Time\n";
    cout << "---------------------------------------\n";
    for (const auto& p : processes) {
        cout << p.id << "\t: " << waiting_times[p.id] << endl;
        total_waiting_time += waiting_times[p.id];
    }
    cout << "---------------------------------------\n";
    cout << "Average Waiting Time = " << fixed << setprecision(2)
        << (double)total_waiting_time / processes.size() << endl;
}

// main 함수
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    string filepath = argv[1]; // 명령줄 인수로 파일 경로 전달
    vector<Process> processes = get_processes_from_file(filepath);

    sjf_scheduling(processes);

    return 0;
}
