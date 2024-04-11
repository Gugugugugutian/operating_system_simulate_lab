#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "size.h"
using namespace std;
int tick = 0;   // 表示时间

// 内存页面
struct page
{
    int pageNumber = 0;                 // 页号
    int disk_pn = -1;                       // 在磁盘中的位置
    int dirty = 0;                      // 修改位（在物理内存中）/ 使用标记（在磁盘外存中）
    bool valid = false;                      // 有效位（在物理内存中）
    int data[PAGE_SIZE];                // 页面存储的数据

    //在物理内存中的页面需要一些其他数据
    int pid = -1;         // 对应进程的id
    int vpn = -1;         // 对应页表的物理页号
    int in_time = 0;      // 加入物理内存的时间
    int hit_time = 0;     // 上次命中的时间
    int ref_sign = 1;     // Clock算法中的引用位

    // 初始化一个页面
    page(){
        for(int& d: data) {
            d = 0;
        }
    };
    page(int pageNumber): pageNumber(pageNumber){
        for(int& d: data) {
            d = 0;
        }
    };
};

// 物理内存
struct memory
{
    page data[P_MEM_SIZE];              // 物理内存中的数据

    // 初始化物理内存
    memory(){
        for(int i=0; i<P_MEM_SIZE; i++) {
            page newPage(i);
            data[i] = newPage;
        }
    };
};
memory PhysicalMemory;

struct disk
{
    page data[DISK_SIZE];              // 磁盘中的数据

    // 初始化物理内存
    disk(){
        for(int i=0; i<P_MEM_SIZE; i++) {
            page newPage(i);
            data[i] = newPage;
            data[i].disk_pn = i;
        }
    };
};
disk Disk;            // 磁盘/外存

// 内存页面调用过程的模拟
// 从外存向物理内存调一个页面
void fromDisk(disk &d = Disk, memory &m = PhysicalMemory, const int ppn = 0, const int address = 0){
    // 如果磁盘上的页面存在且未使用
    if(address < DISK_SIZE) {
        // 标记磁盘的页面为使用（这里dirty作为磁盘页面的使用标记）
        Disk.data[address].dirty++;
        // 复制磁盘的页面到内存
        m.data[ppn] = Disk.data[address];
        // 设置页面为有效
        m.data[ppn].valid = true;
        m.data[ppn].in_time = tick;
        m.data[ppn].disk_pn = address;
    } else {
        cout << "[fromDisk] Address " << address << " out of range. " << endl;
    }
}
// 页面写回
void toDisk(disk &d = Disk, memory &m = PhysicalMemory, const int ppn = 0, const int address = 0){
    // 如果磁盘上的页面存在且已使用
    if(address < DISK_SIZE && Disk.data[address].dirty >= 1) {
        // 复制内存的页面到磁盘
        Disk.data[address] = m.data[ppn];
        // 标记磁盘的页面为未使用
        Disk.data[address].dirty--;
        // 设置页面为无效
        m.data[ppn].valid = false;
    } else {
        cout << "[toDisk] Address " << address << " out of range. " << endl;
    }
}

// 从文件中读入磁盘数据
/*  存储方式：
    以每一页为一行，每一个数字为一个数据的形式，顺序存储在test_pmemdata.txt文件中。
    没有写出的数字全部以0表示
*/
void readDisk(disk &mem = Disk, std::string datadir = "test_disk.txt") {
    std::ifstream file(datadir);
    if (!file.is_open()) {
        std::cerr << "[Read Physical Memory] Failed to open " << datadir << std::endl;
        return;
    }
    int pageNumber = 0;
    int dataIndex = 0;
    while (!file.eof() && pageNumber < DISK_SIZE) {
        int value;
        file >> value;
        mem.data[pageNumber].data[dataIndex++] = value;
        if (dataIndex == PAGE_SIZE) {
            // 下一页
            pageNumber++;
            dataIndex = 0;
        }
    }
    file.close();
}

// 展示物理内存的内容
void show(const memory &mem = PhysicalMemory) {
    std::cout << "[MEMORY SHOW]" << std::endl;
    std::cout << "Valid\tPage\tProcess\tVpn\tDisk\tData\t" << std::endl;
    for (int i = 0; i < P_MEM_SIZE; ++i) {
        const page &currentPage = mem.data[i];
        std::cout << currentPage.valid << "\t" << i << "\t" << currentPage.pid << "\t" << currentPage.vpn << "\t" << currentPage.disk_pn << "\t";
        for (int j = 0; j < PAGE_SIZE; ++j) {
            std::cout << currentPage.data[j] << "\t";
        }
        std::cout << std::endl;
    }
}
// 展示磁盘中的内容
void show(const disk &d = Disk){
    std::cout << "[DISK SHOW]" << std::endl;
    std::cout << "Dirty\t" << "Page\t" << "Data\t" << std::endl;
    for (int i = 0; i < DISK_SIZE; ++i) {
        const page &currentPage = d.data[i];
        std::cout << currentPage.dirty << "\t" << currentPage.pageNumber << "\t";
        for (int j = 0; j < PAGE_SIZE; ++j) {
            std::cout << currentPage.data[j] << "\t";
        }
        std::cout << std::endl;
    }
}
