#ifndef INITSYNTAXANALYZER_H
#define INITSYNTAXANALYZER_H

#include <vector>
#include <set>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Global.h>
using namespace std;

// void readToReader(const string& filePath);
// string initLine(string line);
// int stackPush(int stackTop, const Production& production);
// void initProductions();
// void initMap();
// void processSymbolsToVnSetAndVtSet();
// void output();
// string getMapKey(const string& terminal, const string& nonTerminal);

// 对输入行进行预处理，将特定格式的字符串转换为标准化的形式
// 主要用于将词法分析输出的行转换为“编号,符号”格式
string initLine(const string& line) {
    if (line.empty()) {
        return line;
    }

    // 特殊情况处理
    if (line == ",  <SE,29>") {
        return "29,,";
    }

    string processed = line;

    // 去除行首的制表符
    size_t tabPos = processed.find('\t');
    if (tabPos != string::npos) {
        processed = processed.substr(tabPos + 1);
    }

    // 查找尖括号和逗号，提取token类型和编号
    size_t ltPos = processed.find('<');
    size_t commaPos = processed.find(',');
    size_t gtPos = processed.find('>');

    if (ltPos != string::npos && commaPos != string::npos) {
        string tokenType = processed.substr(ltPos + 1, commaPos - ltPos - 1);

        // 处理标识符、整型常量、浮点常量
        if (tokenType == "IDN") {
            return "30,Ident";
        } else if (tokenType == "INT") {
            return "31,IntConst";
        } else if (tokenType == "FLOAT") {
            return "32,floatConst";
        }
        // 处理其他类型，根据编号查找符号
        if (commaPos != string::npos && gtPos != string::npos) {
            string keyStr = processed.substr(commaPos + 1, gtPos - commaPos - 1);
            try {
                int keyNum = stoi(keyStr);
                auto it = map_i2s.find(keyNum);
                if (it != map_i2s.end()) {
                    return keyStr + "," + it->second;
                }
            } catch (const exception& e) {
                // 转换失败，返回原始行
                return processed;
            }
        }
    }

    // 默认返回处理后的行
    return processed;
}

// 读取指定文件，将每行的第一个逗号前的整数提取并存入 readerVector
void readToReader(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "无法打开文件: " << filePath << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        // 对每一行进行预处理（如去除多余字符、格式转换等）
        string processedLine = initLine(line);

        // 使用逗号分隔，提取第一个字段
        istringstream iss(processedLine);
        string firstField;
        if (getline(iss, firstField, ',')) {
            try {
                // 尝试将第一个字段转换为整数并存入 readerVector
                int value = stoi(firstField);
                readerVector.push_back(value);
            } catch (const exception& e) {
                cerr << "字符串转整数失败: " << e.what() << endl;
            }
        }
    }

    file.close();
}

// 将产生式右部符号压入分析栈，并返回新的栈顶位置
int stackPush(int stackTop, const Production& production) {
    // 移除当前栈顶元素（规约时弹出左部）
    stackVector.pop_back();

    // 若产生式右部为空或为"$"（空产生式），仅弹出栈顶，无需压入新符号
    if (production.right_string.empty() || production.right_string[0] == "$") {
        return stackTop - 1;
    }

    // 逆序将产生式右部符号压入栈（保证左端先出）
    for (auto it = production.right_string.rbegin(); it != production.right_string.rend(); ++it) {
        stackVector.push_back(*it);
    }

    // 返回新的栈顶位置
    return stackTop + static_cast<int>(production.right_string.size()) - 1;
}

void initMap() {
    // 符号到编号的映射
    map_s2i = {
            {"int", 1}, {"void", 2}, {"return", 3}, {"const", 4}, {"main", 5},
            {"float", 6}, {"if", 7}, {"else", 8},{"+", 9}, {"-", 10}, {"*", 11},
            {"/", 12}, {"%", 13}, {"=", 14},{">", 15}, {"<", 16},{"!",17},
            {"==", 18}, {"<=", 19}, {">=", 20}, {"!=", 21},
            {"&&", 22}, {"||", 23}, {"(", 24}, {")", 25}, {"{", 26}, {"}", 27},
            {";", 28}, {",", 29}, {"Ident", 30}, {"IntConst", 31},{"floatConst",32} ,
            {"#", 33}

    };
    // 编号到符号的映射
    map_i2s = {
            {1, "int"}, {2, "void"}, {3, "return"}, {4, "const"}, {5, "main"},{6, "float"},
            {7, "if"}, {8, "else"},{9, "+"}, {10, "-"}, {11, "*"}, {12, "/"}, {13, "%"},
            {14, "="},{15, ">"}, {16, "<"},{17, "!"}, {18, "=="}, {19, "<="}, {20, ">="},
            {21, "!="},{22, "&&"}, {23, "||"}, {24, "("}, {25, ")"}, {26, "{"}, {27, "}"},
            {28, ";"}, {29, ","}, {30, "Ident"}, {31, "IntConst"}, {32,"floatConst"},{33,"#"}

    };
}

// 提取文法中的非终结符(Vn)和终结符(Vt)，并构建产生式映射表 production_map
// 1. 遍历所有产生式，将左部加入VnSet，并将右部字符串加入production_map[left]
// 2. 再遍历所有右部符号，若不在VnSet中，则加入VtSet
void processSymbolsToVnSetAndVtSet() {
    // 第一步：收集非终结符和产生式映射
    for (int i = 0; i < GRAMMAR_SIZE; ++i) {
        const string& prodStr = productionsArray[i]->production_string;
        size_t arrowPos = prodStr.find("->");
        if (arrowPos == string::npos) continue;

        // 提取左部和右部，并去除首尾空格
        string left = prodStr.substr(0, arrowPos);
        string right = prodStr.substr(arrowPos + 2);

        // 去除左部和右部的首尾空格
        left = left.substr(left.find_first_not_of(" "), left.find_last_not_of(" ") - left.find_first_not_of(" ") + 1);
        right = right.substr(right.find_first_not_of(" "), right.find_last_not_of(" ") - right.find_first_not_of(" ") + 1);

        // 将右部字符串加入对应左部的产生式列表
        production_map[left].push_back(right);

        // 左部一定是非终结符
        VnSet.insert(left);
    }

    // 第二步：收集终结符
    for (const auto& [left, rights] : production_map) {
        for (const string& right : rights) {
            istringstream iss(right);
            string symbol;
            while (iss >> symbol) {
                // 如果该符号不是非终结符，则为终结符
                if (VnSet.find(symbol) == VnSet.end()) {
                    VtSet.insert(symbol);
                }
            }
        }
    }
}

// 从指定的文法文件读取产生式，并初始化产生式数组和符号集
// 文法文件每行为: left -> right1 right2 ...（左部和右部用"->"分隔，右部符号以空格分隔）
void initProductions() {
    ifstream file("../syntax_analysis/Grammar"); // 打开文法文件
    if (!file.is_open()) {
        cerr << "无法打开文法文件: ../syntax_analysis/Grammar" << endl;
        return;
    }

    string line;
    int index = 0; // 产生式数组下标
    while (getline(file, line)) {
        // 跳过空行
        if (line.empty()) continue;

        // 查找"->"分隔符
        size_t arrowPos = line.find("->");
        if (arrowPos == string::npos) {
            cerr << "无效的产生式格式: " << line << endl;
            continue;
        }

        // 提取左部（去除多余空格）
        string left = line.substr(0, arrowPos);
        left = left.substr(left.find_first_not_of(" "), left.find_last_not_of(" ") - left.find_first_not_of(" ") + 1);

        // 提取右部（去除多余空格）
        string right = line.substr(arrowPos + 2);
        right = right.substr(right.find_first_not_of(" "), right.find_last_not_of(" ") - right.find_first_not_of(" ") + 1);

        // 分割右部为符号列表
        istringstream iss(right);
        vector<string> rightSymbols;
        string symbol;
        while (iss >> symbol) {
            rightSymbols.push_back(symbol);
        }

        // 创建产生式对象并存入数组
        productionsArray[index++] = new Production(left, rightSymbols, line);
    }
    file.close();

    // 初始化非终结符集、终结符集和产生式映射表
    processSymbolsToVnSetAndVtSet();
}

#endif //INITSYNTAXANALYZER_H
