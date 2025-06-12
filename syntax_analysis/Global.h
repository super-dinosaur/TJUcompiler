#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "Production.h"

// FIRST, FOLLOW, 产生式等全局变量声明
extern std::unordered_map<std::string, std::unordered_set<std::string>> FirstSet;        // First 集合
extern std::unordered_map<std::string, std::unordered_set<std::string>> FirstSetBeta;    // β 的 First 集合
extern std::unordered_map<std::string, std::vector<std::string>> production_map;         // 文法产生式映射
extern std::unordered_set<std::string> VnSet;                                           // 非终结符集合
extern std::unordered_set<std::string> VtSet;                                           // 终结符集合
extern std::unordered_map<std::string, std::unordered_set<std::string>> FollowSet;       // Follow 集合
extern std::string start;                                                               // 文法开始符号

// 预测分析表及辅助数据结构
extern std::map<std::string, int> predictionTable;                                      // LL(1) 预测表
#define GRAMMAR_SIZE 98
extern Production* productionsArray[GRAMMAR_SIZE];                                       // 产生式数组
extern std::vector<std::string> stackVector;                                            // 分析栈
extern std::vector<int> readerVector;                                                   // 读取队列
extern std::map<int, std::string> map_i2s;                                              // 符号编号到字符串映射
extern std::map<std::string, int> map_s2i;                                              // 字符串到符号编号映射

#endif // GLOBAL_H
