#include "memory.h"
#include <vector>

// 页表项
struct pt_item
{
    bool valid = false;   // 页表项有效位

    int vpn = 0;          // 虚拟页号
    int ppn = 0;          // 物理页号
    int disk_pn = 0;      // 数据在磁盘中的位置

    int in_time = 0;      // 加入物理内存的时间
    int hit_time = 0;     // 上次命中的时间
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
    int pid = -1;             // pid
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

int alg = 0;    // 使用的进程调度算法
int clk = 0;    // 时钟算法用到的时钟

// 从文件读入进程和进程的大小 
void readProcess(vector<process> &p = pr_s, const string path="test_process.txt");

// 从文件读入访问顺序
void readAccess(vector<access> &a = access_s, const string& file_path = "test_access.txt");

// 展示所有进程和进程的页表
void showPgTables(const pt& pt) {
    cout << "Valid\t" << "VPN\t" << "PPN\t" << "Hits time\t" << endl;
    for (int i = 0; i < pt.size; ++i) {
        const auto& item = pt.items[i];
        cout << item.valid << "\t" << item.vpn << "\t" << item.ppn << "\t" << item.hit_time << endl;
    }
    cout << endl;
}
void showProcesses() {
    for (const auto& process : pr_s) {
        cout << "Process ID: " << process.pid << ", Name: " << process.name << endl;
        cout << "Page Table:" << endl;
        showPgTables(process.process_pt);
    }
}

// 更新页表：将页表的第vpn页物理页号设置为ppn，标记为有效。
void updatePageTable(process& p, int vpn, int ppn){
    pt& pageTable = p.process_pt;
    // 确保vpn在页表范围内
    if (vpn >= 0 && vpn < pageTable.size) {
        // 更新页表项
        pageTable.items[vpn].ppn = ppn;
        pageTable.items[vpn].valid = true;
        // 更新页表项的时间信息
        pageTable.items[vpn].in_time = tick;
        pageTable.items[vpn].hit_time = tick;
        // 更新物理块的信息
        PhysicalMemory.data[ppn].pid = p.pid;
        PhysicalMemory.data[ppn].vpn = vpn;
        PhysicalMemory.data[ppn].in_time = tick;
        PhysicalMemory.data[ppn].hit_time = tick;
        PhysicalMemory.data[ppn].ref_sign = 1;
    } else {
        std::cerr << "Error: VPN " << vpn << " out of range." << std::endl;
    }
}

// 页面替换：将物理内存的ppn页放回外存old_a地址，从new_a地址取回新的一页放回ppn
void replacePage(memory &mem, disk &d, int ppn, int old_a, int new_a){
    // 确保 ppn 在物理内存范围内
    if (ppn >= 0 && ppn < P_MEM_SIZE) {
        // 将物理内存中的页面写回磁盘
        if(old_a >=0 && old_a < DISK_SIZE) {
            // 将物理块对应的原来的页表项设为无效
            if(PhysicalMemory.data[ppn].valid) {
                int tpid = PhysicalMemory.data[ppn].pid;
                int tvpn = PhysicalMemory.data[ppn].vpn;
                process& tp = pr_s[tpid];
                tp.process_pt.items[tvpn].valid = false;
                cout << "[PageTable] pid: " << tpid << ", Updated: Vpn-" << tvpn << " to Invalid. " << endl;
            }
            toDisk(d, mem, ppn, old_a);
        }
        // 从外存调入新的页面到物理内存
        if(new_a >=0 && new_a < DISK_SIZE) {
            fromDisk(d, mem, ppn, new_a);
        }
        else {
            cerr << "Error: New Address out of range." <<endl;
        }
    } else {
        std::cerr << "Error: PPN " << ppn << " out of range." << std::endl;
    }
}

// 检查访问进程是否存在
bool checkAccess(const access& a) {
    bool exists = false;
    for (const process& p : pr_s) {
        if (a.pid == p.pid) {
            // 进程存在
            exists = true;
            break; // 已经找到，可以提前结束循环
        }
    }
    return exists;
}

// 从物理内存中找出空页面(valid = 0)，返回物理页号
int getEmptyPage(const memory &mem = PhysicalMemory) {
    for (int i = 0; i < P_MEM_SIZE; ++i) {
        if (!mem.data[i].valid) {
            // 找到空页面，返回物理页号
            return i;
        }
    }
    // 没有找到空页面，返回 -1 表示失败
    return -1;
}

// 使用FIFO算法实现调度
// void FIFO(const access& a, vector<process>& p = pr_s);
// void FIFO(const vector<access>& accs = access_s){
//     cout << "FIFO: " << endl;
//     cout << "Time\tPid\tAddress\tVPN\tResult\tPPN\tData" << endl;
//     for(const access& a: accs){
//         if(!checkAccess(a)) {
//             // 对应进程不存在，跳过这条指令
//             continue;
//         } else {
//             // 执行这条指令
//             FIFO(a);
//             // show(PhysicalMemory);
//             // showPgTables(pr_s[a.pid].process_pt);
//         }
//         tick++;
//     }
// }

// 使用不同算法实现调度（通用函数）
void schedule(const access& a, const int &al = alg, vector<process>& p = pr_s);
void schedule(const int& al = alg, const vector<access>& accs = access_s){
    switch (al)
    {
    case 0:
        cout << "FIFO: " << endl;
        break;
    case 1:
        cout << "RS: " << endl;
        break;
    case 2:
        cout << "LRU: " << endl;
        break;
    case 3:
        cout << "CLOCK: " << endl;
        break;
    default:
        return;
    }
    cout << "Time\tPid\tAddress\tVPN\tResult\tPPN\tData" << endl;
    for(const access& a: accs){
        if(!checkAccess(a)) {
            // 对应进程不存在，跳过这条指令
            continue;
        } else {
            // 执行这条指令
            schedule(a, al);
            // show(PhysicalMemory);
            // showPgTables(pr_s[a.pid].process_pt);
        }
        tick++;
    }
}