#ifndef PREDICT_TABLE_H
#define PREDICT_TABLE_H

#include "Global.h"    // 依赖全局 FirstSet、FollowSet、productionsArray 等
#include <vector>
#include <string>
#include <unordered_set>
#include <iostream>

// 生成预测表键：终结符_非终结符
inline std::string getMapKey(const std::string &term, const std::string &nonTerm) {
    return term + "_" + nonTerm;
}

// 计算右侧符号串的 First 集 (_computeFirstBeta)
inline std::unordered_set<std::string> computeFirstToRightsSymbols(const std::vector<std::string> &symbols) {
    std::unordered_set<std::string> result;
    for (auto const &sym : symbols) {
        // 若为终结符，直接加入并终止
        if (VtSet.count(sym)) {
            result.insert(sym);
            return result;
        }
        // 若为非终结符，合并其 First
        auto it = FirstSet.find(sym);
        if (it == FirstSet.end()) {
            std::cerr << "Non-terminal not in FirstSet: " << sym << std::endl;
            break;
        }
        const auto &fs = it->second;
        result.insert(fs.begin(), fs.end());
        // 不含 ε 则终止
        if (!fs.count("$")) return result;
    }
    // 移除 ε
    result.erase("$");
    return result;
}

// 构建预测分析表
inline void setPrediction() {
    // 遍历所有产生式
    for (int i = 0; i < GRAMMAR_SIZE; ++i) {
        const auto &prod = *productionsArray[i];
        const auto &rhs = prod.right_string;
        // 跳过空产生式
        if (rhs.empty() || rhs[0] == "$") continue;

        auto firstBeta = computeFirstToRightsSymbols(rhs);
        for (auto const &t : firstBeta) {
            if (t == "$") {
                // 对应 Follow(A)
                for (auto const &f : FollowSet[prod.left_symbol]) {
                    predictionTable[getMapKey(f, prod.left_symbol)] = i;
                }
            } else {
                predictionTable[getMapKey(t, prod.left_symbol)] = i;
            }
        }
    }
    // 空产生式规则补充
    for (int i = 0; i < GRAMMAR_SIZE; ++i) {
        const auto &prod = *productionsArray[i];
        if (!prod.right_string.empty() && prod.right_string[0] == "$") {
            for (auto const &f : FollowSet[prod.left_symbol]) {
                predictionTable[getMapKey(f, prod.left_symbol)] = i;
            }
        }
    }
}

// 从读入和栈顶获取预测产生式索引
inline int getProductionIndex(int readerTop, int stackTop) {
    if (readerTop < 0 || readerTop >= static_cast<int>(readerVector.size()) ||
        stackTop  < 0 || stackTop  >= static_cast<int>(stackVector.size())) {
        std::cerr << "Index out of bounds." << std::endl;
        return -1;
    }
    auto term = map_i2s.at(readerVector[readerTop]);
    auto nonTerm = stackVector[stackTop];
    auto key = getMapKey(term, nonTerm);

    auto it = predictionTable.find(key);
    if (it != predictionTable.end()) {
        // 特殊 Else 分支
        if (nonTerm == "elsePart" && term == "else") return 43;
        return it->second;
    }
    std::cerr << "No entry in predictionTable for key: " << key << std::endl;
    return -1;
}

#endif // PREDICT_TABLE_H
