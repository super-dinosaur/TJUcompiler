#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <string>
#include <vector>

// Production: 文法产生式，包含左侧非终结符、右侧符号序列和原始字符串
struct Production {
    std::string left_symbol;            // 产生式左部
    std::vector<std::string> right_string; // 产生式右部符号列表
    std::string production_string;      // 整条产生式的文本形式

    // 构造：按值接收参数，内部复制保持与原 API 兼容
    Production(const std::string& left,
               const std::vector<std::string>& right,
               const std::string& production)
        : left_symbol(left),
          right_string(right),
          production_string(production) {}
};

#endif // PRODUCTION_H