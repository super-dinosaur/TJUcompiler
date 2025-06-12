# CmmCompiler

## 项目简介
CmmCompiler 是一个针对简化版 C--(C minus minus) 语言的编译器原型，主要实现了以下功能：  
1. **词法分析**：基于 NFA 构建和子集构造算法生成 DFA，并使用 Hopcroft 算法对 DFA 进行最小化；  
2. **语法分析**：加载预测分析表，对词法分析结果进行 LL(1) 语法分析；  
3. **批量测试**：在 `test/` 目录下的示例文件上执行词法和语法分析，并将结果输出到 `results/` 目录。

---

## 目录结构

```
CmmCompiler-main/
├── CMakeLists.txt           # CMake 构建配置
├── main.cpp                 # 程序入口，驱动词法和语法分析流程
├── lexical_analysis/        # 词法分析模块
│   ├── Edge.h
│   ├── DFA.h
│   ├── NFA.h
│   ├── Node.h
│   ├── Token.h
│   ├── Lexer.h
│   └── utils.h
├── syntax_analysis/         # 语法分析模块
│   ├── First.h
│   ├── Follow.h
│   ├── Grammar/             # 文法定义文件
│   ├── Global.h
│   ├── InitSyntaxAnalyzer.h
│   ├── PredictTable.h
│   ├── Production.h
│   └── Syner.h
├── test/                    # 测试用例（.sy 文件）
│   ├── test_1.sy
│   ├── test_2.sy
│   ├── test_3.sy
│   └── ...
├── results/                 # 分析输出结果
│   ├── lexical_analysis_result_of_test_1.sy
│   ├── syntax_analysis_result_of_test_1.sy
│   └── ...
├── build/                   # 构建目录（编译生成的文件将放在此处）
└── README.md                # 本文件
```

---

## 项目构建

1. 创建并进入构建目录：

   ```bash
   mkdir -p build
   cd build
   ```
2. 生成构建文件并编译：

   ```bash
   cmake ..
   make
   ```
3. 编译完成后，可执行文件 `CmmCompiler` 将位于 `build/` 目录下。

4. 可选：如果需要清理编译文件，可以执行：

   ```bash
   make clean
   ```

---

## 项目运行

在 `build/` 目录下执行：

```bash
./CmmCompiler
```

默认情况下，编译器会依次处理 `test/test_1.sy` 到 `test/test_3.sy`，并将生成的词法和语法分析结果写入 `results/` 目录：

* `lexical_analysis_result_of_test_1.sy`
* `syntax_analysis_result_of_test_1.sy`
* `...`

---

## 成员分工
| 任务 | 成员 |
| ---- | ---- |
| 词法分析 | 程洋, 郭睿 |
| 语法分析 | 樊建琦, 姚宇豪 |
| 中间代码生成 | 暂未实现 |
| 模块整合及测试 | 田行健 |