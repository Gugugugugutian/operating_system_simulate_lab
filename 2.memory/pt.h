#include "memory.h"
#include <vector>

// 页表项
struct pt_item
{
    bool valid = false;   // 页表项有效位

    int vpn = 0;          // 虚拟页号
    int ppn = 0;          // 物理页号
    int disk_pn = 0;      // 数据在磁盘中的位置

    int hit_time = 0;     // 上次命中或者修改的时间
    int ref_sign = 1;     // Clock算法中的引用位
    pt_item(){};
};

// 页表
struct pt
{
    int size = 0;
    vector<pt_item> items; // 页表项

    // 初始化一个页表
    pt(){};
    pt(int size): size(size){
        for (int i=0; i<size; i++) {
            pt_item new_pti = {};
            new_pti.vpn = i;            // 分配虚拟页号
            items.push_back(new_pti);
        }
    };
};

// 进程
struct process
{
    const int pid = -1;             // pid
    string name = "undefined";      // name of process
    
    int size = 0;                   // 进程页表的大小
    pt process_pt;                  // 进程的页表

    process(){};
    process(int size): size(size) {
        pt a(size);
        process_pt = a;
    };
};

// 表示各个进程的页表访问顺序
struct access
{
    int pid = -1;   // 进程
    int logicAddress = 0;   // 逻辑地址
    int op = 0; // 操作（ 0 读 | 1 写 )
    int wd = 0; // 写操作写入的数据
};

// 所有进程
vector<process> pr_s = {};
// 访问顺序
vector<access> access_s = {};

// 从文件读入进程和进程的大小 
void readProcess(vector<process> &p = pr_s, const string path="test_process.txt");

// 从文件读入访问顺序
void readAccess(vector<access> &a = access_s, const string& file_path = "test_access.txt");

// 更新页表项：将页表的第vpn页物理页号设置为ppn，标记为有效。
void updatePageTable(pt &pageTable, int vpn, int ppn);

// 页面替换：将物理内存的ppn页放回外存old_a地址，从new_a地址取回新的一页放回ppn
void replacePage(memory &mem, disk &d, int ppn, int old_a, int new_a);

// 

// 使用FIFO算法实现调度
void FIFO();

// 使用RS算法实现调度
void RS();

// 使用LRU算法实现调度
void LRU();

// 使用CLOCK算法实现进程调度
void CLOCK();