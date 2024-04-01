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
        cout << setw(10) << "ID" << setw(20) << "Name" << setw(15) << "Arrive Time" << setw(15) << "Running Time" << setw(15) << "Priority" << endl;
        // 遍历并打印每个进程的信息
        for (size_t i = 0; i < Processes.size(); ++i) {
            const Process& process = Processes[i];
            cout << setw(10) << i + 1 << setw(20) << process.name << setw(15) << process.arrive_time << setw(15) << process.running_time << setw(15) << process.priority << endl;
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
    // Cpu空闲的时间
    float spare = 0;

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
            spare += (processes[index].arrive_time - curTime);
            curTime = processes[index].arrive_time;
        } 
        // 有进程就绪时，执行当前队列的时间最短的进程
        else {
            // 先按运行时间排序，再选出运行时间最短的进程执行
            sortbyRunningTime(readyProcesses);
            Process curProcess = readyProcesses.front();
            cout << "[SJF] Time: " << curTime << "\tProcess " << curProcess.name << " begins. " << endl;
            
            curTime += curProcess.running_time;

            curProcess.turnaround_time = curTime - curProcess.arrive_time;                          // 到达到完成
            curProcess.response_time = curTime - curProcess.running_time - curProcess.arrive_time;  // 到达到响应
            curProcess.waiting_time = curTime - curProcess.running_time - curProcess.arrive_time;   // 等待时间的和
            curProcess.utilization = (curProcess.turnaround_time - curProcess.waiting_time)/curProcess.turnaround_time;
            
            finished.push_back(curProcess);
            readyProcesses.pop();
        }
    }
    // 展示结果
    cout << "[Ending] Time: " << curTime << endl;
    cout << "CPU Utilization Rate: " << 1 - spare/curTime << endl;
    showResults(finished);
}

// 时间片轮转调度算法
void roundRobinScheduling(const std::vector<Process>& originalProcesses, float timeQuantum) {
    // 创建一个副本，用于修改进程状态
    std::vector<Process> processes = originalProcesses;
    // 按照到达时间进行排序
    sortbyArriveTime(processes);
    // 当前时间
    float curTime = 0;
    // 进程索引
    int index = 0;
    // 就绪的进程队列
    std::queue<Process> readyProcesses;
    // 完成的进程
    std::vector<Process> finished;
    float spare = 0;

    // 算法的过程
    // 仍有进程没有完成调度, 循环
    while (finished.size() < processes.size()) {
        // 当前时间已经到达的进程更新加入队列
        for (int i = index; (i < processes.size()) && (processes[i].arrive_time <= curTime); i++) {
            readyProcesses.push(processes[i]);
            index++;
        }
        // 没有进程就绪时，等待下一个进程到达
        if (readyProcesses.empty()) {
            readyProcesses.push(processes[index]);
            spare += (processes[index].arrive_time - curTime);
            curTime = processes[index].arrive_time;
        }
        // 有进程就绪时，按照时间片轮转的方式执行进程
        else {
            Process curProcess = readyProcesses.front();
            cout << "[RR] Time: " << curTime << "\tProcess " << curProcess.name << " begins. " << endl;
            readyProcesses.pop();

            // 判断时间片是否足够执行完当前进程
            if (curProcess.running_time <= timeQuantum) {
                // 当前时间加上当前进程的运行时间
                curTime += curProcess.running_time;
                // 更新进程的属性
                curProcess.turnaround_time = curTime - curProcess.arrive_time;                      // 周转时间
                curProcess.waiting_time = curProcess.turnaround_time - curProcess.running_time;     // 等待时间
                curProcess.response_time = curProcess.waiting_time;                                 // 响应时间即等待时间
                curProcess.utilization = curProcess.running_time / curProcess.turnaround_time;      // 利用率
                // 添加进程到完成队列
                finished.push_back(curProcess);
            } else {
                // 当前时间加上一个时间片
                curTime += timeQuantum;
                // 更新当前进程的运行时间和剩余时间
                curProcess.running_time -= timeQuantum;
                // 当前时间已经到达的进程更新加入队列
                for (int i = index; (i < processes.size()) && (processes[i].arrive_time <= curTime); i++) {
                    readyProcesses.push(processes[i]);
                    index++;
                }
                // 将进程重新加入队列
                readyProcesses.push(curProcess);
            }
        }
    }

    // 展示完成的进程
    cout << "[Ending] Time: " << curTime << endl;
    cout << "CPU Utilization Rate: " << 1 - spare/curTime << endl;
    showResults(finished);
}

// 多级队列调度算法
void multiLevelFeedbackQueueScheduling(const vector<Process>& originalProcesses, vector<float> timeQuantums) {
    // 按照到达时间进行排序
    vector<Process> t = originalProcesses;
    sortbyArriveTime(t);
    // 创建一个能够记录队列的进程列表
    vector<MLFQ> processes = {};
    for(Process t1: t) {
        MLFQ tempMLFQ = {};
        tempMLFQ.p = t1;
        processes.push_back(tempMLFQ);
    }
    // 当前时间
    float curTime = 0;
    // 进程索引
    int index = 0;
    // 就绪的进程队列
    queue<MLFQ> readyProcesses;
    // 完成的进程
    vector<MLFQ> finished;

    float spare = 0;

    // 算法的过程
    while (finished.size() < processes.size()) {
        // 将到达时间小于等于当前时间的进程加入就绪队列
        while (index < processes.size() && processes[index].p.arrive_time <= curTime) {
            readyProcesses.push(processes[index]);
            index++;
        }
        // 如果就绪队列非空，则执行就绪队列中的进程
        if (!readyProcesses.empty()) {
            sortMLFQ(readyProcesses);
            MLFQ curMLFQ = readyProcesses.front();
            cout << "[MLFQ] Time: " << curTime << "\tProcess " << curMLFQ.p.name << " begins. " << endl;
            readyProcesses.pop();

            // 获取当前进程所在队列的时间片大小
            float timeQuantum = timeQuantums[curMLFQ.q];

            // 判断当前队列的时间片是否足够执行完当前进程
            if (curMLFQ.p.running_time <= timeQuantum) {
                // 更新当前进程的属性
                curTime += curMLFQ.p.running_time;
                curMLFQ.p.turnaround_time = curTime - curMLFQ.p.arrive_time;
                curMLFQ.p.waiting_time = curMLFQ.p.turnaround_time - curMLFQ.p.running_time;
                curMLFQ.p.response_time = curMLFQ.p.waiting_time;
                curMLFQ.p.utilization = curMLFQ.p.running_time / curMLFQ.p.turnaround_time;

                // 将当前进程加入完成队列
                finished.push_back(curMLFQ);
            } else {
                // 一个时间片的运行逻辑
                curTime += timeQuantum;
                curMLFQ.p.running_time -= timeQuantum;
                // 将当前进程加入下一优先级队列
                curMLFQ.q = min(curMLFQ.q + 1, static_cast<unsigned>(timeQuantums.size() - 1));
                readyProcesses.push(curMLFQ);
            }
        } else {
            // 如果就绪队列为空，将时间推进到下一个进程的到达时间
            spare += (processes[index].p.arrive_time - curTime);
            curTime = processes[index].p.arrive_time;
        }
    }

    // 展示完成的进程
    cout << "[Ending] Time: " << curTime << endl;
    cout << "CPU Utilization Rate: " << 1 - spare/curTime << endl;
    showResults(finished);
}

// 展示结果
void showResults(const vector<Process>& results){
    float totalTurnaroundTime = 0;
    float totalResponseTime = 0;
    float totalWaitingTime = 0;
    float totalUtilization = 0;
    cout << setw(10) << "Process Name" << setw(25) <<
            "Turnaround time" << setw(20) << 
            "Response time" << setw(20) << 
            "Waiting time" << setw(15) <<
            "Utilization" << endl;
    for (const Process process: results) {
        cout << setw(10) << process.name << setw(25) <<
                process.turnaround_time << setw(20) << 
                process.response_time << setw(20) << 
                process.waiting_time << setw(15) << 
                process.utilization << endl;
        totalTurnaroundTime += process.turnaround_time;
        totalResponseTime += process.response_time;
        totalWaitingTime += process.waiting_time;
        totalUtilization += process.utilization;
    }
    float averageTurnaroundTime = totalTurnaroundTime / results.size();
    float averageResponseTime = totalResponseTime / results.size();
    float averageWaitingTime = totalWaitingTime / results.size();
    float averageUtilization = totalUtilization / results.size();
    cout << endl << "Average Turnaround Time: " << averageTurnaroundTime << " ms" << endl;
    cout << "Average Response Time: " << averageResponseTime << " ms" << endl;
    cout << "Average Waiting Time: " << averageWaitingTime << " ms" << endl;
    cout << "Average Utilization: " << fixed << setprecision(2) << averageUtilization * 100 << " %" << endl;
}
void showResults(const vector<MLFQ>& results){
    float totalTurnaroundTime = 0;
    float totalResponseTime = 0;
    float totalWaitingTime = 0;
    float totalUtilization = 0;
    cout << setw(10) << "Process Name" << setw(25) <<
            "Turnaround time" << setw(20) << 
            "Response time" << setw(20) << 
            "Waiting time" << setw(15) <<
            "Utilization" << endl;
    for (const MLFQ& mlfq : results) {
        const Process& process = mlfq.p;
        cout << setw(10) << process.name << setw(25) <<
                process.turnaround_time << setw(20) << 
                process.response_time << setw(20) << 
                process.waiting_time << setw(15) << 
                process.utilization << endl;
        totalTurnaroundTime += process.turnaround_time;
        totalResponseTime += process.response_time;
        totalWaitingTime += process.waiting_time;
        totalUtilization += process.utilization;
    }
    float averageTurnaroundTime = totalTurnaroundTime / results.size();
    float averageResponseTime = totalResponseTime / results.size();
    float averageWaitingTime = totalWaitingTime / results.size();
    float averageUtilization = totalUtilization / results.size();
    cout << endl << "Average Turnaround Time: " << averageTurnaroundTime << " ms" << endl;
    cout << "Average Response Time: " << averageResponseTime << " ms" << endl;
    cout << "Average Waiting Time: " << averageWaitingTime << " ms" << endl;
    cout << "Average Utilization: " << fixed << setprecision(2) << averageUtilization * 100 << " %" << endl;
}