#include "process.cpp"

// 进程文件的路径
string process_filename = "testrr.txt";

int main() {
    readProcessesFromFile(process_filename, Processes);
    showProcesses(Processes);
    roundRobinScheduling(Processes, 4);
}