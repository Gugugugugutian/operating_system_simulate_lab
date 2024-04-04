#include <vector>
#include <string>

// 数据文件
struct file_dat
{
    int size = 0;   // 文件大小（文件数）
    std::string file_name = "undefined";  // 文件名
    std::vector<int> content = {};  // 文件内容

    file_dat(std::string file_name):file_name(file_name){};  // 文件初始化
};

// 文本文件
struct file_txt
{
    int size = 0;   // 文件大小（文件数）
    std::string file_name = "undefined";  // 文件名
    std::vector<std::string> content = {};  // 文件内容

    file_txt(std::string file_name):file_name(file_name){};  // 文件初始化
};

// 文件夹
struct folder
{
    std::string name = "undefined";   // 文件夹名
    std::vector<folder*> folders = {};  // 文件夹中的其他文件夹
    std::vector<file_dat*> dats = {};  // 文件夹中的数据文件
    std::vector<file_txt*> txts = {};  // 文件夹中的文本文件

    folder(std::string name):name(name){};  // 文件夹初始化
};

folder Root("Root");    // 根目录
std::vector<folder> folders;
std::vector<file_txt> txts;
std::vector<file_dat> dats;

std::string files_dir = "files";    // 文件的存储目录