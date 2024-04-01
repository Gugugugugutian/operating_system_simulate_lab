#include "process.h"

// 从filename文件中，读入进程并记录到程序中
void readProcessesFromFile(const string& filename, vector<Process>& Processes) {
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            Process process;
            if (iss >> process.name >> process.arrive_time >> process.running_time >> process.priority) {
                Processes.push_back(process);
            } else {
                cerr << "Error: Invalid process data in file." << endl;
            }
        }
        file.close();
    } else {
        cerr << "Error: Unable to open file " << filename << endl;
    }
}

// 展示所有的进程
void showProcesses(const vector<Process>& Processes) {
    if (!Processes.empty()) {
        // 打印表头
        cout << setw(10) << "ID" << setw(20) << "Name" << setw(15) << "Arrive Time" << setw(15) << "Running Time" << setw(15) << "Priority" << setw(20) << "State" << endl;
        // 遍历并打印每个进程的信息
        for (size_t i = 0; i < Processes.size(); ++i) {
            const Process& process = Processes[i];
            cout << setw(10) << i + 1 << setw(20) << process.name << setw(15) << process.arrive_time << setw(15) << process.running_time << setw(15) << process.priority << setw(20) << state_to_string(process.state) << endl;
        }
    } else {
        cout << "No processes to show." << endl;
    }
}