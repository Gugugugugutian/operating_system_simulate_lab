#include "pagetable.cpp"

int main(){
    readProcesses();
    showPgTables();
    readOrder();
    LRU();
    showPhysicalMemory();
}