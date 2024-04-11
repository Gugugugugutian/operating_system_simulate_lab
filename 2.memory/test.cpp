#include "pt.cpp"

string dataPath = "test_disk.txt";
string processPath = "test_process.txt";
string accessPath = "test_access.txt";

// 用户选择磁盘调度算法
bool chooseAlgo(int& i = alg){
    cout << "Select algorithm to use when page fault occurs: " << endl
         << " [0] FIFO\n [1] RS\n [2] LRU \n [3] CLOCK " << endl;
    cin >> i;
    switch (i)
    {
    case 0:
        FIFO();
        break;
    case 1:
        // RS();
        break;
    case 2:
        // LRU();
        break;
    case 3:
        // CLOCK();
        break;
    default:
        cout << "[chooseAlgo] The algorithm you select is invalid. " << endl;
        return false;
        break;
    }
    return true;
}

int main(){
    // 从文件内读入数据、访问顺序和进程
    readDisk(Disk, dataPath);
    // show(Disk);
    readProcess(pr_s, processPath);
    readAccess(access_s, accessPath);
    // 用户选择调度算法
    while(!chooseAlgo());
    // 展示内存
    show(PhysicalMemory);
    showProcesses();
}