#include "pt.h"

// 从文件（默认为test_process.txt)读入进程和页表。
/*  进程存储格式
    进程名 页表的大小 页表第一项的外存地址 页表第二页的外存地址 ... 
*/
void readProcess(vector<process> &p, const string path){
    ifstream inputFile(path);
    if (!inputFile) {
        cerr << "Error: Unable to open file " << path << endl;
        return;
    }
    string line;
    while (getline(inputFile, line)) {
        string pname;
        int psize;
        istringstream iss(line);
        if (iss >> pname >> psize){
            // 创建新的进程
            process newProcess(psize);
            newProcess.name = pname;    
            // 录入新的进程的外存地址
            int pageAddr;
            for(int i=0; i < psize && (iss >> pageAddr); i++) {
                if(pageAddr < DISK_SIZE){
                    newProcess.process_pt.items[i].disk_pn = pageAddr;
                }
                else {
                    // 磁盘地址太大，无法分配外存
                    newProcess.process_pt.items[i].disk_pn = -1;
                    cerr << "[Error] Failed to arrange disk space for address: " << pageAddr << "." << endl;
                }
            }
            newProcess.pid = !p.empty()? p.back().pid + 1 : 0;
            p.push_back(newProcess);
        }
    }
    inputFile.close();
}

// 从文件读入访问顺序
/*  访问顺序存储格式：
    进程id 地址 操作数
*/
void readAccess(vector<access> &acc, const string& file_path) {
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << file_path << endl;
        return;
    }
    string line;
    while (getline(file, line)) {
        if(line[0]=='*') continue; // 注释行以*号开头
        istringstream iss(line);
        access a;
        if (iss >> a.pid >> a.logicAddress >> a.op) {
            if (a.op == 1) {
                if (!(iss >> a.wd)) {
                    cerr << "Error: Invalid data encountered in file " << file_path << endl;
                    return;
                }
            }
            acc.push_back(a);
        } else {
            cerr << "Error: Invalid data encountered in file " << file_path << endl;
            return;
        }
    }
    file.close();
}

// 使用FIFO算法实现调度
void FIFO(const access& a, vector<process>& p){
    pt& pageTable = p[a.pid].process_pt;    // 对应进程的页表
    int vpn = a.logicAddress / PAGE_SIZE; // 访问虚拟页号
    int bias = a.logicAddress % PAGE_SIZE;    // 访问的页内偏移量
    int ppn = -1;    // 物理页号

    // 检查页表是否命中
    if (vpn < pageTable.size && pageTable.items[vpn].valid){
        // 页表命中
        ppn = pageTable.items[vpn].ppn;
        cout << tick << "\t" << a.pid << "\t" << a.logicAddress << "\t" << vpn << "\tHIT\t" << ppn << "\t" << PhysicalMemory.data[ppn].data[bias] << endl;
    } 
    else if(vpn < pageTable.size) {
        // 页表未命中, vpn有效
        cout << tick << "\t" << a.pid << "\t" << a.logicAddress << "\t" << vpn << "\tMISS\t" << endl;
        
        // 寻找一个新的空闲块
        ppn = getEmptyPage();
        if(ppn == -1) {
            // 不存在空闲页，调用磁盘调度算法
            ppn = 0;
            // 找到最早加入的物理块
            for(int i=0; i<P_MEM_SIZE; i++){
                page pg = PhysicalMemory.data[i];
                ppn = (pg.in_time < PhysicalMemory.data[ppn].in_time) ? i : ppn;
            }
        }

        // 从外存向ppn调页
        int old_addr = (PhysicalMemory.data[ppn].valid)? PhysicalMemory.data[ppn].disk_pn : -1;
        // show(PhysicalMemory);
        int new_addr = pageTable.items[vpn].disk_pn; 
        replacePage(PhysicalMemory, Disk, ppn, old_addr, new_addr);
        updatePageTable(p[a.pid], vpn, ppn);
        cout << "[PageTable] pid: " << a.pid << ", Updated: Vpn-" << vpn << ", Ppn-" << ppn << ". " << endl;
    } else {
        // 页表未命中， vpn无效
        cout << tick << "\t" << a.pid << "\t" << a.logicAddress << "\t" << vpn << "\tMISS\t" << endl;
        cout << "[PageFault] Failed to handle this page missing, as the virtual page number is too large." << endl;
    }
}