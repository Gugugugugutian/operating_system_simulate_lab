#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
using namespace std;

#define pReady 0;
#define pWaiting 1;
#define pRunning 2;

// 进程文件的路径
string process_filename = "process.txt";

struct Process
{
    /* 表示一个进程 */
    string name;         // 进程的名称
    float arrive_time = 0;   // 进程的到达时间
    float running_time = 0;  // 进程的运行时间
    unsigned priority = 0;   // 进程优先级，用无符号整数表示，最小为0 
    unsigned state = 0;      // 进程的状态
}; 

// 所有的进程
vector<Process> Processes;

// 将进程的状态转为 String, 方便显示
string state_to_string(unsigned state) {
    string s = "undefined";
    switch (state)
    {
    case 0: 
        s = "ready";
        break;
    case 1:
        s = "waiting";
        break;
    case 2:
        s = "running";
        break;
    }
    return s;
}

// 从filename文件中，读入进程并记录到程序中
void readProcessesFromFile(const string& filename, vector<Process>& Processes);

// 展示所有的进程
void showProcesses(const vector<Process>& Processes);
