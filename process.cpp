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
        // sortbyArriveTime(Processes);    // 读入后，按照到达时间排序，方便后续算法的进行
    } else {
        cerr << "Error: Unable to open file " << filename << endl;
    }
}

// 展示所有的进程
void showProcesses(const vector<Process>& Processes) {
    if (!Processes.empty()) {
        // 打印表头
        cout << setw(10) << "ID" << setw(20) << "Name" << setw(15) << "Arrive Time" << setw(15) << "Running Time" << setw(15) << "Priority" << setw(20) << "Turnaround time" << setw(20) << "Response time" << setw(20) << "Waiting time" << setw(15) << "Utilization" << endl;
        // 遍历并打印每个进程的信息
        for (size_t i = 0; i < Processes.size(); ++i) {
            const Process& process = Processes[i];
            cout << setw(10) << i + 1 << setw(20) << process.name << setw(15) << process.arrive_time << setw(15) << process.running_time << setw(15) << process.priority << setw(20) << process.turnaround_time << setw(20) << process.response_time << setw(20) << process.waiting_time << setw(15) << process.utilization << endl;
        }
    } else {
        cout << "No processes to show." << endl;
    }
}

// 短进程优先调度算法
void shortestJobFirstScheduling(const vector<Process>& originalProcesses) {
    // 创建一个副本，用于修改进程状态
    vector<Process> processes = originalProcesses;
    // 按照到达时间进行排序
    sortbyArriveTime(processes);

    // 当前时间
    float curTime = 0;
    // 进程索引
    int index = 0;
    // 就绪的进程队列
    queue<Process> readyProcesses = {}; 
    // 完成的进程
    vector<Process> finished = {};

    // 算法的过程
    // 仍有进程没有完成调度, 循环
    while(finished.size()<processes.size()){
        // 当前时间已经到达的进程更新加入队列
        for(int i=index; (i<processes.size())&&(processes[i].arrive_time<=curTime); i++) {
            readyProcesses.push(processes[i]);
            index++; 
        }
        // 没有进程就绪时，等待下一个进程到达
        if(readyProcesses.empty()){
            readyProcesses.push(processes[index]);
            curTime = processes[index].arrive_time;
        } 
        // 有进程就绪时，执行当前队列的时间最短的进程
        else {
            // 先按运行时间排序，再选出运行时间最短的进程执行
            sortbyRunningTime(readyProcesses);
            Process curProcess = readyProcesses.front();
            cout << "[SJF] Time: " << curTime << "\tProcess " << curProcess.name << " is running. " << endl;
            curTime += curProcess.running_time;
            curProcess.utilization = 1;
            finished.push_back(curProcess);
            readyProcesses.pop();
        }
    }
    showProcesses(processes);
    // showProcesses(finished);
}