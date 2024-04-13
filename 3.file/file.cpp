#include "file.h"
#include "memory.h"
#include <algorithm>

// 碎片合并操作用到的结构体
struct Merge
{
    bool dirty = false; // 索引项的修改位
    int fileId = -1;    // 文件id
    int folderId = -1;  // 所属的文件夹id
    int start = -1;     // 在存储中的开始位置
    int size = 0;       // 文件的大小

    Merge(){};
    Merge(int fileId, int folderId, int start, int size) : fileId(fileId), folderId(folderId), start(start), size(size){};
};

// 用于碎片合并时排序的算法
bool compareMerge(const Merge &a, const Merge &b)
{
    return a.start < b.start;
}
bool compareMerge1(const Merge &a, const Merge &b)
{
    return a.folderId < b.folderId;
}

// 用于合并碎片时，将两个地址之间的内容提前一段距离
void forwardMemory(int distance, int start, int size, memory &mem = Disk)
{
    int newStart = start - distance;
    for (int i = 0; i < size; i++)
    {
        mem.data[newStart + i] = mem.data[start + i];
    }
}

// 合并文件碎片的尝试
bool mergeMemory(vector<folder> &fs = folders, memory &mem = Disk)
{
    // 遍历文件夹，获取所有文件的信息
    vector<Merge> fileInfo = {};
    for (const folder f : fs)
    {
        for (const file fi : f.files)
        {
            Merge n(fi.id, f.id, fi.head, fi.size);
            fileInfo.push_back(n);
        }
    }
    // 按开始地址排序获取到的文件
    sort(fileInfo.begin(), fileInfo.end(), compareMerge);
    // 没有文件，不需要合并碎片
    if (fileInfo.empty())
    {
        return true;
    }

    // 检查每两个内存上连续的文件之间是否存在碎片，如果存在，将第二个文件提前
    if (fileInfo[0].start != 0)
    {
        // 如果第一个文件前有碎片
        int forward = fileInfo[0].start;
        // 将第一个文件提前
        forwardMemory(forward, fileInfo[0].start, fileInfo[0].size, mem);
        fileInfo[0].start -= forward;
        fileInfo[0].dirty = true;
    }
    for (int i = 1; i < fileInfo.size(); ++i)
    {
        // 如果其他两个文件间有碎片空间
        if (fileInfo[i].start > fileInfo[i - 1].start + fileInfo[i - 1].size)
        {
            // 计算空间的大小
            int gapSize = fileInfo[i].start - (fileInfo[i - 1].start + fileInfo[i - 1].size);
            // 将第i个文件提前
            forwardMemory(gapSize, fileInfo[i].start, fileInfo[i].size, mem);
            fileInfo[i].start -= gapSize;
            fileInfo[i].dirty = true;
        }
    }

    // 根据merge结构体，修改文件夹向量内的内存索引
    for (const Merge &m : fileInfo)
    {
        // 如果merge结构体被修改过
        if (m.dirty)
        {
            // 找到id对应的文件夹
            vector<file> &files = fs[getFolder(m.folderId)].files;
            for (file &f : files)
            {
                // 找到id对应的文件
                if (f.id == m.fileId)
                {
                    // 更新文件索引
                    f.head = m.start;
                }
            }
        }
    }
    memoryBack = fileInfo.back().start + fileInfo.back().size;
    cout << "[Merge] Fragments merged, now the memory is used: " << memoryBack << " / " << MEMORY_SIZE << ". " << endl;
}

// 初始化文件系统, 根目录 id = 0, 如果尝试删除会提示失败
void initFileSystem(vector<folder> &v)
{
    if (globalFolderId == 0)
    {
        // 创建根目录
        folder root("root");
        v.push_back(root);
    }
    else
    {
        // 根目录已经存在，报错
        cout << "[Error] The file system has been initialized. " << endl;
    }
}

// 重置文件系统
void resetFileSystem(vector<folder> &fs)
{
    fs.clear();         // 清除所有文件夹和文件
    globalFolderId = 0; // 重设 global folder ID
    globalFileId = 0;   // 重设 global file ID
    cout << "[SYSTEM] File system reset successfully." << endl;
    initFileSystem(fs);
}

// 切换当前路径
void changeDirectory(const int &dirId, const vector<folder> &fs)
{
    if (getFolder(dirId, fs) != -1)
    {
        currentFolderId = dirId;
    }
    else
    {
        cout << "[Error] The folder you are trying to change to doesn't exist. " << endl;
    }
}

// 新建文件夹
void newFolder(const string &name, const int &dirId, int &id, vector<folder> &fs)
{
    folder &directory = fs[getFolder(dirId)];
    directory.subFolders.push_back(id);
    folder newFolder(name, id);
    fs.push_back(newFolder);
}

// 新建文件
void newFile(const string &name, const int &size, const int &dirId, int head, int id, vector<folder> &fs)
{
    folder &directory = fs[getFolder(dirId)];
    // 如果head没有输入，计算新文件的起始地址
    if (head < 0)
    {
        head = memoryBack;
    }
    if (head + size < MEMORY_SIZE)
    {
        // 文件没有超出内存大小限制
        // 创建文件
        file newFile(size, head, name, id);
        // 添加文件到现有路径
        directory.files.push_back(newFile);
        // 更新内存尾端
        memoryBack = head + size;
    }
    else
    {
        // 合并文件碎片后继续尝试放入
        mergeMemory();
        head = memoryBack;
        if (head + size < MEMORY_SIZE)
        {
            // 碎片清除后可以创建文件
            // 创建文件
            file newFile(size, head, name, id);
            // 添加文件到现有路径
            directory.files.push_back(newFile);
            // 更新内存尾端
            memoryBack = head + size;
        }
        else
        {
            cout << "[Error] Failed to create file, as the memory is not big enough. " << endl;
        }
    }
}

// 对一个文件夹做列表操作
void listFolder(const folder &f)
{
    cout << "List: [" << f.id << "] " << f.folderName << endl;
    show(f);
}
void listFolder(const int &id)
{
    // 如果id不存在，则list根目录
    listFolder(folders[max(getFolder(id), 0)]);
}
