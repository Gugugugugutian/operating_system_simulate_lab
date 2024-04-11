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

// 更新页表项：将页表的第vpn页物理页号设置为ppn，标记为有效。
void updatePageTable(pt &pageTable, int vpn, int ppn){
    // 确保vpn在页表范围内
    if (vpn >= 0 && vpn < pageTable.size) {
        // 更新页表项
        pageTable.items[vpn].ppn = ppn;
        pageTable.items[vpn].valid = true;
    } else {
        std::cerr << "Error: VPN " << vpn << " out of range." << std::endl;
    }
}

// 页面替换：将物理内存的ppn页放回外存old_a地址，从new_a地址取回新的一页放回ppn
void replacePage(memory &mem, disk &d, int ppn, int old_a, int new_a) {
    // 确保 ppn 在物理内存范围内
    if (ppn >= 0 && ppn < P_MEM_SIZE) {
        // 将物理内存中的页面写回磁盘
        toDisk(d, mem, ppn, old_a);
        // 从外存调入新的页面到物理内存
        fromDisk(d, mem, ppn, new_a);
    } else {
        std::cerr << "Error: PPN " << ppn << " out of range." << std::endl;
    }
}

