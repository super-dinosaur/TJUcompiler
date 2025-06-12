#ifndef FOLLOW_H
#define FOLLOW_H

#include "Global.h"  
#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <iterator>

// 计算所有非终结符的 Follow 集合
void Follow() {
    // 初始：清空所有 Follow
    for (auto const& vn : VnSet) {
        FollowSet[vn].clear();
    }
    // 文法开始符号加入 '#'
    FollowSet[start].insert("#");

    bool changed = true;
    while (changed) {
        changed = false;
        // 遍历每条产生式 A → α
        for (auto const& [A, productions] : production_map) {
            for (auto const& prod : productions) {
                auto symbols = splitSymbols(prod);
                // 遍历产生式右侧每个符号 B
                for (size_t i = 0; i < symbols.size(); ++i) {
                    auto const& B = symbols[i];
                    if (!VnSet.count(B)) continue;  // 仅处理非终结符 B

                    auto& followB = FollowSet[B];
                    bool allNullable = true;
                    // 处理 B 后续的 β 右部
                    for (size_t j = i + 1; j < symbols.size(); ++j) {
                        auto const& beta = symbols[j];
                        // 若 β 是终结符，直接加入
                        if (!VnSet.count(beta)) {
                            if (followB.insert(beta).second) changed = true;
                            allNullable = false;
                            break;
                        }
                        // 若 β 是非终结符，加入其 First(β)\{ε}
                        for (auto const& t : FirstSet[beta]) {
                            if (t != "$" && followB.insert(t).second) {
                                changed = true;
                            }
                        }
                        // 若 First(β) 不含 ε，则停止，再无空串
                        if (!FirstSet[beta].count("$")) {
                            allNullable = false;
                            break;
                        }
                    }
                    // 若 B 之后均可空，将 A 的 Follow 加入 B
                    if (allNullable) {
                        for (auto const& t : FollowSet[A]) {
                            if (followB.insert(t).second) {
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

#endif // FOLLOW_H