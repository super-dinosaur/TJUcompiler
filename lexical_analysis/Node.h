#ifndef NODE_H
#define NODE_H

#include <string>
#include <utility>

// Node: NFA/DFA 状态表示，包含 ID、是否接受、是否回滚需求及类型标签
class Node {
private:
    int id;                     // 状态编号
    bool isLast;                // 是否为终止状态
    bool needRollback;          // 是否需回退上一字符
    std::string type;           // 状态类型标签

public:
    // 构造函数：按值传入类型字符串，并内部移动
    Node(int id, bool isLast, bool needRollback, std::string type)
        : id(id), isLast(isLast), needRollback(needRollback), type(std::move(type)) {}

    // 访问器
    int getId() const noexcept            { return id; }
    bool getIsLast() const noexcept       { return isLast; }
    bool getIsNeedRollback() const noexcept { return needRollback; }
    const std::string &getType() const noexcept { return type; }

    // 用于集合与排序操作
    bool operator<(const Node &other) const noexcept { return id < other.id; }
    bool operator==(const Node &other) const noexcept { return id == other.id; }
};

#endif // NODE_H
