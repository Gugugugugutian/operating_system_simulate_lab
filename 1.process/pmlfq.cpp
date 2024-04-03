#include "process.cpp"

// 进程文件的路径
string process_filename = "testmlfq.txt";

int main() {
    readProcessesFromFile(process_filename, Processes);
    showProcesses(Processes);
    multiLevelPreemptiveFeedbackQueueScheduling(Processes, {1,2,4});
}