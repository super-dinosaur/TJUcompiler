#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// 读取指定路径文件内容并打印日志
inline std::string readFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Cannot Open File: " << filePath << std::endl;
        return {};
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    std::cout << ">>> READ FILE: " << filePath << std::endl
              << content << std::endl
              << std::string(80, '-') << std::endl;
    return content;
}

#endif // UTILS_H