#include "process.cpp"

// 进程文件的路径
string process_filename = "processes50.txt";

int main() {
    readProcessesFromFile(process_filename, Processes);
    showProcesses(Processes);
    multiLevelPreemptiveFeedbackQueueScheduling(Processes, {1,2,4});
}