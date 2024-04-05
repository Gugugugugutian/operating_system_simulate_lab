#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "size.h"

// 内存页面
struct page
{
    int pageNumber = 0;                 // 页号
    int dirty = 0;                      // 修改位
    int data[PAGE_SIZE];                // 页面存储的数据

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
struct physi_memory
{
    page data[P_MEM_SIZE];              // 物理内存中的数据

    // 初始化物理内存
    physi_memory(){
        for(int i=0; i<P_MEM_SIZE; i++) {
            page newPage(i);
            data[i] = newPage;
        }
    };
};

physi_memory PhysicalMemory;            // 程序中的物理内存

// 从文件中读入内存
/*  内存存储方式：
    内存以每一页为一行，每一个数字为一个数据的形式，顺序存储在test_pmemdata.txt文件中。
    没有写出的数字全部以0表示
*/
void readPhysicalMemory(physi_memory &mem = PhysicalMemory, std::string datadir = "test_pmemdata.txt") {
    std::ifstream file(datadir);
    if (!file.is_open()) {
        std::cerr << "[Read Physical Memory] Failed to open " << datadir << std::endl;
        return;
    }
    int pageNumber = 0;
    int dataIndex = 0;
    while (!file.eof() && pageNumber < P_MEM_SIZE) {
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
void showPhysicalMemory(const physi_memory &mem = PhysicalMemory) {
    std::cout << "[Physical Memory]" << std::endl;
    std::cout << "Dirty\t" << "Page\t" << "Data\t" << std::endl;
    for (int i = 0; i < P_MEM_SIZE; ++i) {
        const page &currentPage = mem.data[i];
        std::cout << currentPage.dirty << "\t" << currentPage.pageNumber << "\t";
        for (int j = 0; j < PAGE_SIZE; ++j) {
            std::cout << currentPage.data[j] << "\t";
        }
        std::cout << std::endl;
    }
}