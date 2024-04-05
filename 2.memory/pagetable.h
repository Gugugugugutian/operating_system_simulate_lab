#include "memory.h"
#include <iostream>
#include <vector>
using namespace std;

int tick = 0;   // 表示执行的时间，每执行一个访问则+1

// 页表项
struct pagetableitem
{
    int id = -1;                        // 页表项的序号
    bool valid = false;                 // 页表项有效位

    int physical_page_number = 0;          // 物理页号
    int virtual_page_number = 0;           // 虚拟页号

    int hit_time = 0;       // 上次命中或者修改的时间

    pagetableitem(){};
};

// 页表
struct pagetable
{
    pagetableitem items[PAGE_TABLE_SIZE]; // 页表项

    // 初始化一个页表
    pagetable(){
        for (int i=0; i<PAGE_TABLE_SIZE; i++) {
            items[i].id = i;
        }
    };
};

// 进程，包含进程的页表
struct Process
{
    const int pid = 0;                        // 进程id
    string name = "undefined";          // 进程名称
    pagetable pt;                       // 进程的页表

    Process(int pid): pid(pid){};
};

vector<Process> processes = {};         // 程序的所有进程，每个进程有一个页表 

int getPage();

// 创建进程
void createProcess(string pname, int pid = 0) {
    // 使pid转为有效数字
    pid = ((!processes.empty())&&(pid<processes.back().pid)) ? processes.back().pid: pid;  
    Process newProcess(pid);
    newProcess.name = pname;
    processes.push_back(newProcess);
};

// 展示所有进程和进程的页表
void showPgTables(const pagetable& pt) {
    cout << "ID\t" << "Valid\t" << "VPN\t" << "PPN\t" << "Updated/Hits time\t" << endl;
    for (int i = 0; i < PAGE_TABLE_SIZE; ++i) {
        const auto& item = pt.items[i];
        cout << item.id << "\t" << item.valid << "\t" << item.virtual_page_number << "\t" << item.physical_page_number << "\t" << item.hit_time << endl;
    }
    cout << endl;
}
void showPgTables() {
    for (const auto& process : processes) {
        cout << "Process ID: " << process.pid << ", Name: " << process.name << endl;
        cout << "Page Table:" << endl;
        showPgTables(process.pt);
    }
}

// 替换页表中的第i项
int replacePtItem(pagetable& pt, const int& i, const int& vpn) {
    int ppn = getPage();    // 从内存中自动请求一个页
    int tppn = pt.items[i].physical_page_number;
    // 释放被替换的页表项
    if(pt.items[i].valid)
    {
        PhysicalMemory.data[tppn].dirty = 0;    // 修改位置0
    }
    // 修改页表项
    pt.items[i].virtual_page_number = vpn;  // 新的虚拟页号
    pt.items[i].physical_page_number = ppn;
    pt.items[i].valid = 1;
    pt.items[i].hit_time = tick;
    cout << "[PageTable] Updated: Index-" << i << ", Vpn-" << vpn << ", Ppn-" << ppn << ". " << endl;

    
    return ppn;
}

// 从物理内存中取一个未使用的页，返回页号
int getPage() {
    for(page& p: PhysicalMemory.data) {
        // 有未使用的页面，返回页号
        if(p.dirty == 0) {
            p.dirty = 1;
            return p.pageNumber;
        }
    }
    // 没有未使用的页面，说明物理内存不够大，程序报错
    cerr << "[Memory] Error: All pages occupied." << endl;
    return -1;
}

// 从文件（默认为test_processes.txt)读入进程和页表。
/*  进程存储格式
    进程名 页表第一项的物理页号 页表第二项的物理页号 ...
*/
void readProcesses(const string& file_path = "test_processes.txt") {
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << file_path << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string processName;
        iss >> processName;
        Process newProcess(!processes.empty()? processes.back().pid + 1 : 0);
        newProcess.name = processName;

        int physicalPage = 0;
        int virtualPage = 0; 
        int pageIndex = 0;
        while (iss >> virtualPage >> physicalPage && pageIndex < PAGE_TABLE_SIZE) {
            newProcess.pt.items[pageIndex].physical_page_number = physicalPage;
            newProcess.pt.items[pageIndex].virtual_page_number = virtualPage;
            newProcess.pt.items[pageIndex].valid = true;
            newProcess.pt.items[pageIndex].hit_time = tick;
            PhysicalMemory.data[physicalPage].dirty = 1;
            pageIndex++;
        }
        processes.push_back(newProcess);
    }
    file.close();
}