#include "pagetable.cpp"

int main(){
    readProcesses();
    showPgTables();
    readOrder();
    FIFO();
    showPhysicalMemory();
}