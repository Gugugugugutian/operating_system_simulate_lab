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
    float remaining_time = 0;  // 进程的剩余时间
    unsigned priority = 0;   // 进程优先级，用无符号整数表示
    /* 用于计算平均周转时间/平均带权周转时间，平均等待时间、平均响应时间、利用率 */
    float turnaround_time = 0;  // 周转时间
    float waiting_time = 0;     // 等待时间
    float response_time = -0.25;    // 响应时间
    float utilization = 0;      // 利用率
};

// 多级队列调度算法时，需要保存一些临时变量，使用这个结构体
struct MLFQ
{
    Process p;               // 进程本身
    unsigned q = 0;          // 当前进程所处的队列
    unsigned preem_time = 0; // 当前进程被抢占前，已经执行当前时间片的时间
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

// 将一个队列的进程按剩余运行时间排序
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
        return a.remaining_time < b.remaining_time;
    });
    // 将向量返回队列
    for (const Process& p : tempVec) {
        Ps.push(p);
    }
}

// 短进程优先调度算法
void shortestJobFirstScheduling(const vector<Process>& originalProcesses);

// 将一个多级队列按所处的队列顺序
void sortMLFQ(queue<MLFQ>& Ps) {
    // 临时向量，用于存储并排序
    vector<MLFQ> tempVec;
    // 从队列中取出进程放到向量中
    while (!Ps.empty()) {
        tempVec.push_back(Ps.front());
        Ps.pop();
    }
    // 排序
    sort(tempVec.begin(), tempVec.end(), [](const MLFQ& a, const MLFQ& b) {
        return a.q < b.q;
    });
    // 将向量返回队列
    for (const MLFQ& p : tempVec) {
        Ps.push(p);
    }
}

// 多级队列调度算法
void multiLevelFeedbackQueueScheduling(const vector<Process>& originalProcesses, vector<float> timeQuantums);

// 有抢占的多级队列调度
void multiLevelPreemptiveFeedbackQueueScheduling(const vector<Process>& originalProcesses, vector<float> timeQuantums);

// 展示结果
void showResults(const vector<Process>& results);
void showResults(const vector<MLFQ>& results);