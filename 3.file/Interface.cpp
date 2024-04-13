#include "file.cpp"
#include <iomanip>

string dataPath = "./data/memory.txt";  // 内存数据存储路径
string treePath = "./data/tree.txt";    // 文件树结构存储路径

// 展示当前路径
void showCurrentDir(const int &id = currentFolderId, const vector<folder> &fs = folders)
{
    for (folder f : fs)
    {
        if (f.id == id)
        {
            cout << ":) [" << f.id << "]_" << f.folderName << " >>\t";
        }
    }
}

// 展示开始界面
void startInterface()
{
    initFileSystem();
    cout << "\n\n[fileSystem] Welcome to file system! " << endl;
    cout << "/help: get help. " << endl;
    showCurrentDir();
}

// 帮助界面
void help()
{
    // 打开文件
    ifstream file("commands.txt");
    if (!file.is_open())
    {
        cout << "Unable to open file!" << endl;
        return;
    }
    string command, description;
    cout << "\n\n[HELP]" << endl;
    // 逐行读取文件内容
    while (getline(file, command) && getline(file, description))
    {
        // 输出指令和说明
        cout << command << ": " << description << endl;
    }
    // 关闭文件
    file.close();
    cout << "\n\n\n"
         << endl;
}

// 命令的解析, 如果退出界面则返回true, 否则返回false.
bool parsing(string &line)
{
    // 使用 stringstream 分割命令
    stringstream ss(line);
    string action;               // 命令操作
    vector<string> options = {}; // 命令的其他参数

    // 提取命令和参数
    ss >> action;
    string option;
    while (ss >> option)
    {
        options.push_back(option);
    }

    // 解析和执行命令
    if (action == "/help")
    {
        help();
    }
    else if (action == "/dbg")
    {
        // 调试指令
        string t = options[0];
        switch (t[0])
        {
        case 'm':
            // 内存的调试
            showMemory();
            break;
        case 'f':
            // 文件的调试
            show();
        case 'g':
            // 碎片合并算法的调试
            mergeMemory();
            break;
        default:
            break;
        }
    }
    else if (action == "/quit")
    {
        return true; // 执行退出
    }
    else if (action == "/load")
    {
        loadData(dataPath);
        loadFiles(treePath);
    }
    else if (action == "/save")
    {
        saveData(dataPath);
        saveFiles(treePath);
    }
    else if (action == "/list")
    {
        if(!options.empty()){
            // list 指定ID的目录
            listFolder(stoi(options[0]));
        }
        else {
            // list 当前目录
            listFolder(currentFolderId);
        }
    }
    else if (action == "/new")
    {
        switch (options.size())
        {
        case 1:
            // new foldername 的情况
            newFolder(options[0]);
            break;
        case 2:
            // new filename size的情况
            newFile(options[0], stoi(options[1]));
            break;
        case 3:
            // new -d folderid foldername的情况
            if(options[0] == "-d") 
            {
                newFolder(options[2], stoi(options[1]));
            }
            break;
        case 4:
            // new -d folderid filename size的情况
            if(options[0] == "-d") 
            {
                newFile(options[2], stoi(options[3]), stoi(options[1]));
            }
            break;
        default:
            break;
        }
    }
    else if (action == "/cd")
    {
        if (!options.empty())
        {
            int newDirectory = stoi(options[0]);
            changeDirectory(newDirectory);
        }
        else{
            cout << "[Error] Try: /cd 0" << endl;
        }
    }
    else
    {
        // 输入不为空
        if (action != "/" && action != "")
        {
            // 输入错误
            cout << "[Error] The command you input is not found. ";
            if (action[0] != '/')
            {
                // 提示用户输入左斜线
                cout << "A correct instruction usually starts with a '/' in this system. ";
            }
            cout << "Try: '/help' is you'd like to get help. " << endl;
        }
    }
    return false; // 不退出
}

// 文件系统的界面逻辑
void interface()
{
    startInterface();
    string command = ""; // 用户输入的命令
    bool quit = false;   // 用户退出

    while (!quit && getline(cin, command))
    {
        quit = parsing(command);
        showCurrentDir();
    }
    cout << "[fileSystem] Exiting... " << endl;
}

int main()
{
    interface();
}