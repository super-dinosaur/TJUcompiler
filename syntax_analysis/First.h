#ifndef FIRST_H
#define FIRST_H

#include "Global.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <iterator>

// 分割以空格分隔的符号串
static std::vector<std::string> splitSymbols(const std::string& str) {
    std::istringstream iss(str);
    return { std::istream_iterator<std::string>(iss), {} };
}

// 计算单个符号 ch 的 First 集合
void getFirst(const std::string& ch) {
    auto& firstSet = FirstSet[ch];
    // 若为终结符，则自身即为 First
    if (VtSet.count(ch)) {
        firstSet.insert(ch);
        return;
    }
    // 遍历 ch 的所有产生式
    for (auto const& prod : production_map[ch]) {
        auto symbols = splitSymbols(prod);
        bool allNullable = true;
        for (auto const& sym : symbols) {
            getFirst(sym);
            for (auto const& t : FirstSet[sym]) {
                if (t != "$") firstSet.insert(t);
            }
            // 若当前符号 First 不包含空串，则终止
            if (!FirstSet[sym].count("$")) {
                allNullable = false;
                break;
            }
        }
        if (allNullable) firstSet.insert("$");
    }
}

// 初始化所有非终结符的 First 集合
void First() {
    for (auto const& vn : VnSet) {
        getFirst(vn);
    }
}

// 计算右部串 beta 的 FirstBeta 集合
void getFirstOfBeta(const std::string& beta) {
    auto& firstBeta = FirstSetBeta[beta];
    auto symbols = splitSymbols(beta);
    bool allNullable = true;
    for (auto const& sym : symbols) {
        if (!FirstSet.count(sym)) getFirst(sym);
        for (auto const& t : FirstSet[sym]) {
            if (t != "$") firstBeta.insert(t);
        }
        if (!FirstSet[sym].count("$")) {
            allNullable = false;
            break;
        }
    }
    if (allNullable) {
        firstBeta.insert("$");
    }
}

#endif // FIRST_H