#include "process.cpp"

// 进程文件的路径
string process_filename = "testsjf.txt";

int main() {
    readProcessesFromFile(process_filename, Processes);
    showProcesses(Processes);
    shortestJobFirstScheduling(Processes);
}