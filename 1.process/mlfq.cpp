#include "process.cpp"

// 进程文件的路径
string process_filename = "process.txt";

int main() {
    readProcessesFromFile(process_filename, Processes);
    showProcesses(Processes);
    multiLevelFeedbackQueueScheduling(Processes, {1,2,4});
}