#include "process.cpp"

int main() {
    readProcessesFromFile(process_filename, Processes);
    showProcesses(Processes);
    shortestJobFirstScheduling(Processes);
}