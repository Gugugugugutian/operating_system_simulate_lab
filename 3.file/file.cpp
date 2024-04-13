#include "file.h"
#include "memory.h"

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

// 合并文件碎片的尝试
bool merge(memory& mem = Disk){

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
    else{
        // 合并文件碎片后继续尝试放入
        merge();
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
