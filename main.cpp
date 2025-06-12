#include <iostream>
#include <string>
#include "NFA.h"
#include "DFA.h"
#include "Lexer.h"
#include "Syner.h"  // 重命名后的语法分析头文件

int main() {
    // 构建并最小化 DFA
    NFA nfa;
    DFA dfa(nfa);
    dfa.build();       // 子集构造
    dfa.minimize();    // Hopcroft 最小化

    // 初始化语法分析
    init();

    // 批量处理测试文件
    for (int i = 1; i <= 3; ++i) {
        std::string idx = std::to_string(i);
        std::string cmm = "../test/test_" + idx + ".sy";
        std::cout << "===== Lexical Analysis of test_" << idx << ".sy =====" << std::endl;
        Lexer lexer(cmm, dfa);
        lexer.analyze();
        lexer.outputResult();

        std::cout << "===== Syntax Analysis of test_" << idx << ".sy =====" << std::endl;
        std::string laResult = "../results/lexical_analysis_result_of_test_" + idx + ".sy";
        if (syntax_analysis(laResult) != 0) {
            std::cerr << "Syntax analysis failed for " << laResult << std::endl;
            return 1;
        }
        std::cout << "============================================" << std::endl;
    }

    return 0;
}
