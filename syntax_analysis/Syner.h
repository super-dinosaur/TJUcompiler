#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H

#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include <sstream>
#include <iostream>
#include <fstream>
#include "InitSyntaxAnalyzer.h"
#include "First.h"
#include "Follow.h"
#include "PredictTable.h"

// 全局变量定义
std::unordered_map<std::string, std::unordered_set<std::string>> FirstSet;
std::unordered_map<std::string, std::unordered_set<std::string>> FirstSetBeta;
std::unordered_map<std::string, std::unordered_set<std::string>> FollowSet;
std::unordered_map<std::string, std::vector<std::string>> production_map;
std::unordered_set<std::string> VnSet;
std::unordered_set<std::string> VtSet;
std::map<int, std::string> map_i2s;
std::map<std::string, int> map_s2i;
Production* productionsArray[GRAMMAR_SIZE];
std::vector<std::string> stackVector;
std::vector<int> readerVector;
std::string start;
std::map<std::string, int> predictionTable;

// 初始化所有语法分析相关组件
void init() {
    start = "Program";
    initMap();
    initProductions();
    First();
    for (auto const& vn : VnSet) {
        for (auto const& beta : production_map[vn]) {
            getFirstOfBeta(beta);
        }
    }
    Follow();
    setPrediction();
}

// 主执行函数：进行 LL(1) 语法分析
int syntax_analysis(const std::string& path) {
    // 初始化栈与读入指针
    stackVector.clear(); readerVector.clear();
    stackVector.push_back(std::to_string(map_s2i["#"]));
    stackVector.push_back(start);
    readToReader(path);
    readerVector.push_back(map_s2i["#"]);

    int stackTop = static_cast<int>(stackVector.size()) - 1;
    int readerTop = 0;
    int stepCount = 1;
    std::ostringstream outputBuffer;

    while (stackTop >= 0) {
        // 当前栈顶与读入符号
        std::string symbol = stackVector[stackTop];
        int symCode = std::atoi(symbol.c_str());
        std::string symStr = (symCode != 0 ? map_i2s[symCode] : symbol);
        std::string lookahead = "#" + map_i2s[readerVector[readerTop]];

        // 输出步信息
        std::cout << stepCount << " " << symStr << " " << lookahead;
        outputBuffer << stepCount++ << "\t" << symStr << " " << lookahead;

        // 匹配或规约
        if ((symCode != 0 && symCode == readerVector[readerTop]) || symStr == map_i2s[readerVector[readerTop]]) {
            stackVector.pop_back();
            readerVector.erase(readerVector.begin() + readerTop);
            --stackTop;
            std::cout << " move\n";
            outputBuffer << "\tmove\n";
        } else {
            int prodIndex = getProductionIndex(readerTop, stackTop);
            if (prodIndex < 0) {
                std::cerr << " Error\n";
                return 1;
            }
            stackTop = stackPush(stackTop, *productionsArray[prodIndex]);
            std::cout << " reduction\n";
            outputBuffer << "\treduction\n";
        }
    }

    // 接受状态
    if (stackTop < 0) {
        std::cout << "Accept\n";
        outputBuffer << "Accept\n";
    }

    // 输出结果文件路径替换
    std::string outputPath = path;
    const std::string from = "lexical_analysis";
    const std::string to   = "syntax_analysis";
    auto pos = outputPath.find(from);
    if (pos != std::string::npos) outputPath.replace(pos, from.length(), to);

    std::ofstream outFile(outputPath);
    if (!outFile) {
        std::cerr << "Failed to open output file: " << outputPath << std::endl;
        return 1;
    }
    outFile << outputBuffer.str();
    return 0;
}

#endif // SYNTAX_ANALYZER_H