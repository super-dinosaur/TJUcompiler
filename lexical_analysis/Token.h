#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <sstream>

// Token: 词法单元表示，包括原文、类型和编号
class Token {
private:
    std::string lexeme;   // 词素文本
    std::string type;     // 词法类型
    std::string id;       // 词法编号或原文本

public:
    // 构造：按值接收，内部移动，保持与旧接口一致
    Token(std::string lexeme, std::string type, std::string id)
        : lexeme(std::move(lexeme)), type(std::move(type)), id(std::move(id)) {}

    // 生成可打印字符串
    std::string toString() const {
        std::stringstream ss;
        ss << lexeme << "  <" << type << "," << id << ">";
        return ss.str();
    }
};

#endif // TOKEN_H
