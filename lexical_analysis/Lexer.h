#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <DFA.h>
#include <Token.h>
#include <utils.h>

class Lexer {
private:
    std::string file_path;
    std::string file_content;
    std::vector<Token> tokens;
    DFA dfa;

    // 跳过空白字符判断
    bool isWhitespace(char c) const noexcept {
        return c==' ' || c=='\n' || c=='\t';
    }

    // 提取不含路径的文件名
    std::string extractFileName() const {
        auto pos = file_path.find_last_of("/\\");
        return (pos==std::string::npos ? file_path : file_path.substr(pos+1));
    }

public:
    // 构造函数：文件路径与 DFA 对象按值传入
    Lexer(std::string path, DFA dfa_obj)
        : file_path(std::move(path)), file_content(readFromFile(file_path)), dfa(std::move(dfa_obj)) {}

    // 执行词法分析
    void analyze() {
        std::string curr;
        int lastState = 0;
        auto len = file_content.size();
        for (size_t i = 0; i < len; ++i) {
            char c = file_content[i];
            if (curr.empty() && isWhitespace(c)) continue;

            curr.push_back(c);
            std::string s(1, c);
            if (!dfa.getNextNode(s)) {
                std::cerr << "Lexer error: " << curr << std::endl;
                return;
            }
            lastState = dfa.current_id;
            if (dfa.isFinal(lastState)) {
                if (dfa.needRollback(lastState)) {
                    curr.pop_back();
                    --i;
                }
                auto nodeType = dfa.getType(lastState);
                auto tokenType = dfa.getTokenType(curr, nodeType);
                auto tokenId   = dfa.getTokenId(curr, tokenType);
                tokens.emplace_back(curr, tokenType, tokenId);
                curr.clear();
                dfa.getStart();
            }
        }
        if (!dfa.isFinal(lastState)) {
            std::cerr << "Lexer error: " << curr << " is Not a Complete Token!" << std::endl;
        }
    }

    // 输出分析结果到控制台及文件
    void outputResult() const {
        auto name = extractFileName();
        std::ofstream out("../results/lexical_analysis_result_of_" + name);
        for (auto const& tok : tokens) {
            auto line = tok.toString();
            std::cout << line << std::endl;
            out << line << std::endl;
        }
    }
};

#endif // LEXER_H