#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// 当前文件夹的id
int currentFolderId = 0;

// 全局文件的id
int globalFileId = 0;
// 全局文件夹的id
int globalFolderId = 0;
// 表示用到内存尾端的变量
int memoryBack = 0;

// 文件索引
struct file
{
    // 编号
    const int id = -1;
    // 文件名
    std::string fileName = "undefined";
    // 文件大小
    int size = 0;
    // 文件的起始地址
    int head = 0;
    // 例如，head = 0， size = 16，则文件占用内存的[0]到[15]的空间

    // 文件索引项的初始化
    file(int size, int head, string name = "newFile", int id = globalFileId) : size(size), head(head), id(id), fileName(name)
    {
        globalFileId = max(globalFileId+1, id+1);
        cout << "[SYSTEM] A new file [" << id << "]_" << name << " is created: from address (" << head << ") to (" << head + size - 1 << "). " << endl;
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
    vector<file> files = {};

    // 文件夹的初始化
    folder(string name = "newFolder", int id = globalFolderId) : id(id), folderName(name)
    {
        globalFolderId = max(globalFolderId+1, id+1);
        cout << "[SYSTEM] A new folder [" << id << "]_" << name << " is created. " << endl;
    }
};

// 所有文件夹
vector<folder> folders = {};

// 通过id获取文件夹在vector向量中的索引
int getFolder(const int &id, const vector<folder> &fs = folders)
{
    for (int i = 0; i < fs.size(); i++)
    {
        if (fs[i].id == id)
            return i;
    }
    cout << "[Error] The folder [" << id << "] doesn't exist. " << endl;
    return -1;
}

// 初始化文件系统, 根目录 id = 0, 如果尝试删除会提示失败
void initFileSystem(vector<folder> &v = folders);

// 重置文件系统
void resetFileSystem(vector<folder> &fs = folders);

// 切换当前路径
void changeDirectory(const int& dirId, const vector<folder> &fs = folders);

// 新建文件夹
void newFolder(const string &name, const int &dirId = currentFolderId, int& id = globalFolderId, vector<folder> &fs = folders);

// 新建文件
void newFile(const string &name, const int& size, const int &dirId = currentFolderId, int head = -1,  int id = globalFileId, vector<folder> &fs = folders);

// 从/data文件夹读入文件索引
void loadFiles(const string &path, vector<folder> &fs = folders){
    // 清除现有的文件系统
    resetFileSystem(fs);
    // 切换到根目录
    changeDirectory(0);

    ifstream inputFile(path);
    if (inputFile.is_open()) {
        string line;
        while (getline(inputFile, line)) {
            istringstream iss(line);
            string token;
            iss >> token;
            
            if (token == "in") {
                int parentId, id;
                string type, name;
                iss >> parentId >> type >> id >> name;
                if (type == "folder") {
                    newFolder(name, parentId, id);
                } else if (type == "file") {
                    int head, size;
                    iss >> head >> size;
                    newFile(name, size, parentId, head, id);
                }
            }
        }

        cout << "[SYSTEM] File index loaded successfully from " << path << endl;
        inputFile.close(); // Close the file
    } else {
        cerr << "[Error] Unable to open file: " << path << endl;
    }
}

// 将文件索引写入/data文件夹
void saveFiles(const string &path, vector<folder> &fs = folders){
    ofstream outputFile(path);
    if (outputFile.is_open()){
        changeDirectory(0); // 切换到根目录
        for (const folder& f : fs) {
            // 输出文件夹内文件的信息
            for (const file& fi : f.files) {
                outputFile << "in " << f.id << " file " << fi.id << " " << fi.fileName << " " << fi.head << " " << fi.size << "\n";
            }
            for (const int& folderId: f.subFolders) {
                outputFile << "in " << f.id << " folder " <<  folderId << " " << fs[getFolder(folderId)].folderName << "\n";
            }
        }
        cout << "[SYSTEM] File index saved successfully to " << path << endl;
        outputFile.close();
    } else {
        cerr << "[Error] Unable to create file: " << path << endl;
    }
}

// 展示文件夹中的文件
void show(const vector<file>& fs) {
    cout << "Files:\nID\tSize\tHead\tName\n";
    for (const file& f : fs) {
        cout << f.id << "\t" << f.size << "\t" << f.head << "\t" << f.fileName << "\n";
    }
}
// 展示文件夹
void show(const folder& f, const vector<folder>& fs = folders) {
    cout << "Folder ID: " << f.id << ", Name: " << f.folderName << endl;
    show(f.files);
    cout << "\nFolders In " << f.folderName << ": " << endl;
    for(const int& t: f.subFolders){
        cout << "[" << t << "]\t" << fs[getFolder(t)].folderName << endl;
    }
}
// 展示全部文件夹
void show(const vector<folder>& fs = folders) {
    for (const folder& f : fs) {
        show(f, fs);
    }
}