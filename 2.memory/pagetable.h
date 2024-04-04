#include "memory.h"
#include <iostream>
#include <vector>
using namespace std;

#define PAGE_TABLE_SIZE 4               // 页表的大小，单位为页表项

// 页表项
struct pagetableitem
{
    int id = -1;                        // 页表项的序号
    bool valid = false;                 // 页表项有效位

    int logic_page_number = 0;          // 虚拟页号
    int physi_page_number = 0;          // 物理页号
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

// 创建进程
void createProcess(string pname, int pid = 0) {
    // 使pid转为有效数字
    pid = ((!processes.empty())&&(pid<processes.back().pid)) ? processes.back().pid: pid;  
    Process newProcess(pid);
    processes.push_back(newProcess);
};

vector<int> address_order = {};     // 访问逻辑地址的次序

// 从文件中读入访问次序，文件路径默认为test_access.txt
void readOrder(std::string filepath = "test_access.txt") {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Address Order] Error opening file " << filepath << std::endl;
        return;
    }
    int address;
    while (file >> address) {
        address_order.push_back(address);
    }
    file.close();
}