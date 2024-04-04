#define PAGE_SIZE 16    // 页面的大小，单位为int
#define P_MEM_SIZE 8 // 物理内存的大小，单位为页面数
#include <string>
#include <iostream>
#include <fstream>

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
    int dirty = 0;
    page data[P_MEM_SIZE];              // 物理内存中的数据

    // 初始化物理内存
    physi_memory(): dirty(0){
        for(int i=0; i<P_MEM_SIZE; i++) {
            page newPage(i);
            data[i] = newPage;
        }
    };
};

physi_memory PhysicalMemory;            // 程序中的物理内存


// 调用内存中的一个页
page getPage(physi_memory &mem, int pageNumber) {
    if (pageNumber <= P_MEM_SIZE) {
        return mem.data[pageNumber];
    }
    else return mem.data[0];
}

// 将一个页放回物理内存
void retPage(physi_memory &mem, const page& page) {
    if(page.dirty) {
        mem.data[page.pageNumber] = page;
    }
}

// 从文件中读入内存
/*  内存存储方式：
    内存以每一页为一行，每一个数字为一个数据的形式，顺序存储在test_pmemdata.txt文件中。
    没有写出的数字全部以0表示
*/
void readPhysicalMemory(physi_memory &mem, std::string datadir = "test_pmemdata.txt") {
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
void showPhysicalMemory(const physi_memory &mem) {
    std::cout << "[Physical Memory]" << std::endl;
    for (int i = 0; i < P_MEM_SIZE; ++i) {
        const page &currentPage = mem.data[i];
        std::cout << "Page " << currentPage.pageNumber << ":\t";
        for (int j = 0; j < PAGE_SIZE; ++j) {
            std::cout << currentPage.data[j] << "\t";
        }
        std::cout << std::endl;
    }
}