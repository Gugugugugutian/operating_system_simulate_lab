#include "memory.h"
#include <iostream>
#include <vector>
using namespace std;

int tick = 0;   // 表示执行的时间，每执行一个访问则+1

// 页表项
struct pagetableitem
{
    bool valid = false;                 // 页表项有效位
    const int virtual_page_number = 0;           // 虚拟页号

    int physical_page_number = 0;          // 物理页号
    int disk_page_number = 0;              // 数据在磁盘中的位置

    int hit_time = 0;       // 上次命中或者修改的时间

    pagetableitem(){};
};

// 进程，包含进程的页表
struct Process
{
    const int pid = 0;                  // 进程id
    int size = 0;                       // 进程页表的大小
    string name = "undefined";          // 进程名称
    vector<pagetableitem> pt = {};      // 进程的页表

    Process(int pid, int size): pid(pid), size(size){
        for (int i=0; i<size; i++) {
            pagetableitem pti = {};
            pti.virtual_page_number = i;
            pt.push_back(pti);
        }
    };
};

vector<Process> processes = {};         // 程序的所有进程，每个进程有一个页表 

// 调用进程调度算法
void call(){

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
    // 没有未使用的页面，说明物理内存已经填满，需要调度物理块的替换算法
    cerr << "[Memory] Physical memory is full. " << endl;
    return -1;
}

// 创建进程
void createProcess(string pname, int pid = 0) {
    // 使pid转为有效数字
    pid = ((!processes.empty())&&(pid<processes.back().pid)) ? processes.back().pid: pid;  
    Process newProcess(pid);
    newProcess.name = pname;
    processes.push_back(newProcess);
};

// 展示所有进程和进程的页表
void showPgTables(const vector<pagetableitem>& pt) {
    cout << "Valid\t" << "VPN\t" << "PPN\t" << "Updated/Hits time\t" << endl;
    for (int i = 0; i < P_MEM_SIZE; ++i) {
        const auto& item = pt.items[i];
        cout << item.valid << "\t" << item.virtual_page_number << "\t" << item.physical_page_number << "\t" << item.hit_time << endl;
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

// 设置页表第i项为无效，第j项为有效，并寻找新的物理块（i， j是虚拟页号）
int replacePtItem(vector<pagetableitem>& pt, const int& i, const int& j) {
    if(!pt.items[i].valid || j>=P_MEM_SIZE) {
        cout << "[Pgtable] Error, page number outbound or the origin page is already invalid." << endl;
        return -1;
    }
    pt.items[i].valid = false; // 原页表项设为无效
    int ppn = getPage();    // 从内存中自动请求一个页
    pt.items[j].physical_page_number = ppn;
    pt.items[j].valid = true;  // 新页表项设为有效
    return ppn;
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
        int diskPage = 0;
        int pageIndex = 0;
        while (iss >> virtualPage >> physicalPage >> diskPage && pageIndex < P_MEM_SIZE) {
            newProcess.pt.items[pageIndex].physical_page_number = physicalPage;
            newProcess.pt.items[pageIndex].virtual_page_number = virtualPage;
            newProcess.pt.items[pageIndex].disk_page_number = diskPage;
            newProcess.pt.items[pageIndex].valid = false;
            newProcess.pt.items[pageIndex].hit_time = tick;
            PhysicalMemory.data[physicalPage].dirty = 1;
            pageIndex++;
        }
        processes.push_back(newProcess);
    }
    file.close();
}