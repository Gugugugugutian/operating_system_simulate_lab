#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <queue>
using namespace std;

struct Process
{
    /* 表示一个进程 */
    string name;             // 进程的名称
    float arrive_time = 0;   // 进程的到达时间
    float running_time = 0;  // 进程的运行时间
    unsigned priority = 0;   // 进程优先级，用无符号整数表示，最小为0 
    /* 用于计算平均周转时间/平均带权周转时间，平均等待时间、平均响应时间、利用率 */
    float turnaround_time = 0;  // 周转时间
    float waiting_time = 0;     // 等待时间
    float response_time = 0;    // 响应时间
    float utilization = 0;      // 利用率
};

// 所有的进程
vector<Process> Processes = {};

// 把所有进程按照到达时间排序
void sortbyArriveTime(vector<Process>& Processes) {
    sort(Processes.begin(), Processes.end(), [](const Process& a, const Process& b) {
        return a.arrive_time < b.arrive_time;
    });
}

// 从filename文件中，读入进程并记录到程序中
void readProcessesFromFile(const string& filename, vector<Process>& Processes);

// 展示所有的进程
void showProcesses(const vector<Process>& Processes);

// 时间片轮转调度算法
void roundRobinScheduling(const std::vector<Process>& originalProcesses, float timeQuantum);

// 将一个队列的进程按运行时间排序
void sortbyRunningTime(queue<Process>& Ps) {
    // 临时向量，用于存储并排序
    vector<Process> tempVec;
    // 从队列中取出进程放到向量中
    while (!Ps.empty()) {
        tempVec.push_back(Ps.front());
        Ps.pop();
    }
    // 排序
    sort(tempVec.begin(), tempVec.end(), [](const Process& a, const Process& b) {
        return a.running_time < b.running_time;
    });
    // 将向量返回队列
    for (const Process& p : tempVec) {
        Ps.push(p);
    }
}

// 短进程优先调度算法
void shortestJobFirstScheduling(const vector<Process>& originalProcesses);

// 多级队列调度算法
void multiLevelFeedbackQueueScheduling(const vector<Process>& originalProcesses, vector<float> timeQuantums);