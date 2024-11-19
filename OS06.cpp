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

// ���μ��� ����ü ����
struct Process {
    string id;
    int arrival_time;
    queue<int> cpu_bursts;
    queue<int> io_bursts;
    int waiting_time = 0;  // ���� ��� �ð�
    int remaining_time = 0; // ���� CPU burst ���� �ð�
};

// SJF �켱���� �� (CPU burst ����)
struct Compare {
    bool operator()(const Process* a, const Process* b) {
        return a->remaining_time > b->remaining_time; // ª�� burst�� �켱
    }
};

// ���� �Է� ó��
vector<Process> get_processes_from_file(const string& filepath) {
    vector<Process> processes;
    ifstream input_file(filepath);

    if (!input_file.is_open()) {
        cerr << "Error: Unable to open file " << filepath << endl;
        exit(EXIT_FAILURE);
    }

    // ���� ������ ���
    string line;
    while (getline(input_file, line)) {
        if (line.empty()) continue;
        cout << line << endl; // ���� ���� �״�� ���

        // ���μ��� ���� �Ľ�
        stringstream ss(line);
        Process p;
        string token;

        ss >> p.id; // ���μ��� �̸� �б�

        // ���� �ð� �� CPU/I/O ����Ʈ �б�
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

// SJF �����ٸ� �ùķ��̼�
void sjf_scheduling(const vector<Process>& processes) {
    auto ready_queue = priority_queue<Process*, vector<Process*>, Compare>();
    vector<Process> process_list = processes;
    map<string, int> waiting_times; // ���μ��� ��� �ð� ���
    int current_time = 0; // ���� �ð�
    int completed = 0;    // �Ϸ�� ���μ��� ��
    int total_waiting_time = 0;

    while (completed < process_list.size()) {
        // ���� Ready ���·� ������ ���μ��� �߰�
        for (auto& p : process_list) {
            if (p.arrival_time <= current_time && p.remaining_time == 0 && !p.cpu_bursts.empty()) {
                p.remaining_time = p.cpu_bursts.front(); // ���� CPU burst ��������
                ready_queue.push(&p);
            }
        }

        // CPU ó��
        if (!ready_queue.empty()) {
            Process* current_process = ready_queue.top();
            ready_queue.pop();

            current_process->cpu_bursts.pop(); // ���� CPU burst �Ϸ�
            int burst_time = current_process->remaining_time;
            current_time += burst_time; // ���� �ð��� ����
            current_process->remaining_time = 0;

            // I/O �غ� �Ǿ� ������ ó��
            if (!current_process->io_bursts.empty()) {
                current_process->arrival_time = current_time + current_process->io_bursts.front();
                current_process->io_bursts.pop();
            }
            else {
                // ��� CPU burst�� �����ٸ� ���μ��� ����
                completed++;
                waiting_times[current_process->id] = current_process->waiting_time;
            }

            // �ٸ� ���μ��� ��� �ð� ����
            for (auto& p : process_list) {
                if (p.remaining_time == 0 && p.arrival_time <= current_time && &p != current_process) {
                    p.waiting_time += burst_time;
                }
            }
        }
        else {
            // ���� ������ ���μ����� ������ �ð� ����
            current_time++;
        }
    }

    // ��� ���
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

// main �Լ�
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    string filepath = argv[1]; // ����� �μ��� ���� ��� ����
    vector<Process> processes = get_processes_from_file(filepath);

    sjf_scheduling(processes);

    return 0;
}
