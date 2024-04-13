#include <vector>
#include <string>
#include <fstream>
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
        for(char &d: data){
            d = '/';
        }
    }
};
// 文件存储
memory Disk = {};

// 从文件中读入存储
void loadData(const string& path, memory& mem = Disk) {
    ifstream file(path); // Open the file in text mode
    if (file.is_open()) {
        int i = 0;
        char c;
        while (file.get(c)) { // Read characters from the file
            mem.data[i++] = c;
        }
        file.close(); // Close the file
    } else {
        cerr << "Unable to open file: " << path << endl;
    }
}

// 将存储写回文件
void saveData(const string& path, memory& mem = Disk) {
    ofstream file(path); // Open the file in text mode
    if (file.is_open()) {
        for (int i = 0; i < MEMORY_SIZE; ++i) {
            file << mem.data[i]; // Write characters from memory to file
        }
        file.close(); // Close the file
    } else {
        cerr << "Unable to create file: " << path << endl;
    }
}

// 展示存储
void showMemory(const memory& mem = Disk) {
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        cout << mem.data[i] << " ";
        if ((i + 1) % 16 == 0) { // Print newline after every 16 elements
            cout << endl;
        }
    }
    cout << endl; // Print an additional newline at the end
}