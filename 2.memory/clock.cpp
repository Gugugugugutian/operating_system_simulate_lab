#include "pagetable.cpp"

int main(){
    readProcesses("test_clock_process.txt");
    // showPgTables();
    readOrder();
    CLOCK();
    showPhysicalMemory();
    showPgTables();
}