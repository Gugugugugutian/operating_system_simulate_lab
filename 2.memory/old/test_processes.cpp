#include "pagetable.h"
int main() {
    readPhysicalMemory();
    readProcesses();
    showPhysicalMemory(PhysicalMemory);
    showPgTables();

    return 0;
}