#include <vector>
#include <string>
#include <iostream>

// 存储的大小
#ifndef MEMORY_SIZE
#define MEMORY_SIZE 1024
#endif

using namespace std;

// 存储
struct memory
{
    char data[MEMORY_SIZE];

    // 存储的初始化
    memory(){
        for(char d: data){
            d = '/';
        }
    }
};
// 文件存储
memory Disk = {};
// 全局文件的id
int globalFileId = 0;
// 全局文件夹的id
int globalFolderId = 0;

// 文件索引
struct file_index
{
    // 编号
    const int id = -1;
    // 文件名
    std::string fileName = "undefined";
    // 文件大小
    const int size = 0;
    // 文件的起始地址
    const int head = 0;
    // 例如，head = 0， size = 16，则文件占用内存的[0]到[15]的空间

    // 文件索引项的初始化
    file_index(int size, int head, int id = globalFileId): size(size), head(head), id(id) {
        globalFileId++;
        cout << "[SYSTEM] A new file id:" << id << "is created: from address [" << head << "] to [" << head+size << "]. " << endl;
    }
};

// 树状文件目录的文件夹
struct folder
{
    // 编号
    const int id = -1;
    // 文件夹名
    string folderName = "undefined";
    
    // 存放下级文件夹的id
    vector<int> subFolders = {};
    // 存放文件夹内文件的id
    vector<int> files = {};

    // 文件夹的初始化
    folder(int id = globalFolderId): id(id) {
        globalFolderId++;
        cout << "[SYSTEM] A new folder id:" << id << "is created. " << endl;
    }
};

// 存放所有的文件索引
vector<file_index> files = {};
// 存放所有文件夹
vector<folder> folders = {};