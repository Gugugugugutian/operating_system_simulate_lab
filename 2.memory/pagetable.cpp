# include "pagetable.h"

// 表示各个进程的页表访问顺序
struct access
{
    int pid = -1;   // 进程
    int logicAddress = 0;   // 逻辑地址
    int op = 0; // 操作（ 0 读 | 1 写 )
    int wd = 0; // 写操作写入的数据
};

vector<access> accesss_order = {};     // 访问逻辑地址的次序

// 从文件内读入虚拟地址的访问顺序
void readOrder(const string& file_path = "test_access.txt") {
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << file_path << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        access a;
        if (iss >> a.pid >> a.logicAddress >> a.op) {
            if (a.op == 1) {
                if (!(iss >> a.wd)) {
                    cerr << "Error: Invalid data encountered in file " << file_path << endl;
                    return;
                }
            }
            accesss_order.push_back(a);
        } else {
            cerr << "Error: Invalid data encountered in file " << file_path << endl;
            return;
        }
    }
    file.close();
}

// 检查访问进程的存在性
bool checkAccess(const access& a) {
    bool r = false;
    for(const Process& p: processes) {
        if(a.pid == p.pid) {
            // 进程存在
            r = true;
        }
    }
    return r;
}

// 使用先进先出的页表调度策略
void FIFO(const access& a, vector<Process>& p = processes){
    pagetable& pt = p[a.pid].pt;    // 对应进程的页表
    tick++;

    int vpn = a.logicAddress / PAGE_SIZE; // virtual page number
    int bias = a.logicAddress % PAGE_SIZE;    // in-page bias
    int ppn = -1;    // physical page number(only shows when hits)

    int hit = -1;   // page table hits
    for(pagetableitem& item: pt.items) {
        if(item.valid == 1 && item.virtual_page_number == vpn) {
            hit = item.id;
            ppn = item.physical_page_number;
            // item.hit_time = tick;
            // FIFO相当于只有调用页面时更新时间参数的LRU算法
        }
    }

    if(hit!=-1) {
        // 如果命中 输出命中结果
        cout << tick << "\t" << a.pid << "\t" << a.logicAddress << "\t" << vpn << "\tHIT\t" << ppn << "\t" << PhysicalMemory.data[ppn].data[bias] << endl;
    } else {
        // 如果不命中，使用FIFO替换页面
        cout << tick << "\t" << a.pid << "\t" << a.logicAddress << "\t" << vpn << "\tMISS\t" << endl;
        // 确定要替换的页表项 
        int replace = 0;
        for(int i=0; i<PAGE_TABLE_SIZE; i++) {
            if(!pt.items[i].valid) {
                // 页表第i项还未使用
                replace = i;
                break;
            }
            else {
                // 如果命中时间更早则替换
                replace = (pt.items[i].hit_time < pt.items[replace].hit_time) ? i : replace;
            }
        }
        // 实现页表项的更新
        ppn = replacePtItem(pt, replace, vpn);
        showPgTables(pt);
        cout << "=================================================" << endl;    
        cout << "Time\tPid\tAddress\tVPN\tResult\tPPN\t" << endl;
    }

    // 完成内存的写操作 
    if(a.op == 1) {
        // 写操作
        PhysicalMemory.data[ppn].data[bias] = a.wd;
    }
}
void FIFO(){
    cout << "FIFO: " << endl;
    cout << "Time\tPid\tAddress\tVPN\tResult\tPPN\tData" << endl;
    for(const access& a: accesss_order){
        if(!checkAccess(a)) {
            // 对应进程不存在，跳过这条指令
            continue;
        } else {
            // 执行这条指令
            FIFO(a);
        }
    }
}

// 使用LRU的方式访问地址
void LRU(const access& a, vector<Process>& p = processes){
    pagetable& pt = p[a.pid].pt;    // 对应进程的页表
    tick++;

    int vpn = a.logicAddress / PAGE_SIZE; // virtual page number
    int bias = a.logicAddress % PAGE_SIZE;    // in-page bias
    int ppn = -1;    // physical page number(only shows when hits)

    int hit = -1;   // page table hits
    for(pagetableitem& item: pt.items) {
        if(item.valid == 1 && item.virtual_page_number == vpn) {
            hit = item.id;
            ppn = item.physical_page_number;
            item.hit_time = tick;
        }
    }

    if(hit!=-1) {
        // 如果命中 输出命中结果
        cout << tick << "\t" << a.pid << "\t" << a.logicAddress << "\t" << vpn << "\tHIT\t" << ppn << "\t" << PhysicalMemory.data[ppn].data[bias] << endl;
    } else {
        // 如果不命中，使用LRU替换页面
        cout << tick << "\t" << a.pid << "\t" << a.logicAddress << "\t" << vpn << "\tMISS\t" << endl;
        // 确定要替换的页表项 
        int replace = 0;
        for(int i=0; i<PAGE_TABLE_SIZE; i++) {
            if(!pt.items[i].valid) {
                // 页表第i项还未使用
                replace = i;
                break;
            }
            else {
                // 如果命中时间更早则替换
                replace = (pt.items[i].hit_time < pt.items[replace].hit_time) ? i : replace;
            }
        }
        // 实现页表项的更新
        ppn = replacePtItem(pt, replace, vpn);
        showPgTables(pt);
        cout << "=================================================" << endl;    
        cout << "Time\tPid\tAddress\tVPN\tResult\tPPN\t" << endl;
    }

    // 完成内存的写操作 
    if(a.op == 1) {
        // 写操作
        PhysicalMemory.data[ppn].data[bias] = a.wd;
    }
}
void LRU(){
    cout << "LRU: " << endl;
    cout << "Time\tPid\tAddress\tVPN\tResult\tPPN\tData" << endl;
    for(const access& a: accesss_order){
        if(!checkAccess(a)) {
            // 对应进程不存在，跳过这条指令
            continue;
        } else {
            // 执行这条指令
            FIFO(a);
        }
    }
}

// 使用CLOCK算法依次访问地址
void CLOCK(){

}

// 使用随机置换算法访问地址
void RS(){

}
