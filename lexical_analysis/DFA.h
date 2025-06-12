#ifndef DFA_H
#define DFA_H

#include <NFA.h>
#include <queue>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>

class DFA {
private:
    // 定义类型到内容的映射
    map<string, int> TYPE_TO_CONTENT_DICT_KW = {
            {"int", 1}, {"void", 2}, {"return", 3}, {"const", 4},
            {"main", 5}, {"float", 6}, {"if", 7}, {"else", 8}
    };

    // 定义操作符和分隔符的映射
    map<string, int> TYPE_TO_CONTENT_DICT_OP = {
            {"+", 9}, {"-", 10}, {"*", 11}, {"/", 12}, {"%", 13},
            {"=", 14}, {">", 15}, {"<", 16}, {"!", 17},{"==", 18}, {"<=", 19},
            {">=", 20}, {"!=", 21}, {"&&", 22}, {"||", 23}
    };

    // 定义分隔符的映射
    map<string, int> TYPE_TO_CONTENT_DICT_SE = {
            {"(", 24}, {")", 25}, {"{", 26}, {"}", 27}, {";", 28}, {",", 29}
    };

public:
    vector<Node> node_list;
    vector<Edge> edge_list;
    NFA nfa;
    int current_id;
    int start_id;

    DFA(NFA nfa) {
        this->nfa = std::move(nfa);
        this->current_id = 0;
        this->start_id = 0;
    }

    // 计算节点集合的epsilon闭包
    set<Node> epsilonClosure(set<Node> nodeSet) {
        queue<Node> q;
        // 将初始节点集合中的所有节点加入队列
        for (const auto& n : nodeSet) q.push(n);

        while (!q.empty()) {
            Node top = q.front();
            q.pop();
            // 遍历NFA的所有边，查找epsilon边
            for (const auto& e : nfa.edge_list) {
                // 如果当前节点有一条epsilon边
                if (top.getId() == e.getFromNodeId() && e.getTag() == "epsilon") {
                    Node toNode = nfa.node_list[e.getToNodeId()];
                    // 如果目标节点已经在集合中，则跳过
                    if (nodeSet.find(toNode) != nodeSet.end()) continue;
                    // 否则将目标节点加入集合和队列
                    nodeSet.insert(toNode);
                    q.push(toNode);
                }
            }
        }
        return nodeSet;
    }

    set<Node> move(const set<Node>& nodeSet, const string& a) {
        set<Node> res;
        for (const auto& n : nodeSet) {
            for (const auto& e : nfa.edge_list) {
                if (n.getId() == e.getFromNodeId() && e.getTag() == a) {
                    res.insert(nfa.node_list[e.getToNodeId()]);
                }
            }
        }
        return res;
    }

    // 子集构造法，将NFA确定化为DFA
    void build() {
        // 添加DFA的初始节点
        this->node_list.emplace_back(0, false, false, "");
        set<Node> startNodeSet;
        // NFA的初始节点加入集合
        startNodeSet.insert(nfa.node_list[0]);
        // 计算初始节点的epsilon闭包
        startNodeSet = epsilonClosure(startNodeSet);

        // 用于存储DFA中每个状态对应的NFA节点集合
        vector<set<Node>> node_array_list;
        node_array_list.push_back(startNodeSet);

        size_t pointer = 0;
        int id = 0;
        // 遍历所有已发现的DFA状态
        while (pointer < node_array_list.size()) {
            set<Node> nodeSet = node_array_list[pointer];
            // 对每个输入符号进行转移
            for (const auto& tag : nfa.tags) {
                // 计算当前状态集合经过tag转移后的新状态集合
                set<Node> moveNodeSet = move(epsilonClosure(nodeSet), tag);
                if (moveNodeSet.empty()) continue;

                // 检查该集合是否已存在于DFA状态集中
                auto it = std::find(node_array_list.begin(), node_array_list.end(), moveNodeSet);
                if (it == node_array_list.end()) {
                    // 新集合，加入DFA状态集
                    node_array_list.push_back(moveNodeSet);
                    Node first_in_set = *moveNodeSet.begin();
                    id++;
                    // 新建DFA节点，属性取集合中第一个节点
                    this->node_list.emplace_back(id, first_in_set.getIsLast(), first_in_set.getIsNeedRollback(), first_in_set.getType());
                    // 添加DFA边
                    this->edge_list.emplace_back(pointer, id, tag);
                } else {
                    // 已存在，直接添加边
                    int to_new_node_id = distance(node_array_list.begin(), it);
                    this->edge_list.emplace_back(pointer, to_new_node_id, tag);
                }
            }
            pointer++;
        }
    }

    // 最小化DFA（Hopcroft算法简化版）
    void minimize() {
        // P为划分集合，初始分为终态F和非终态NF
        std::vector<std::set<Node>> P;
        std::set<Node> F, NF;
        for (const auto& node : this->node_list) {
            if (node.getIsLast())
                F.insert(node);      // 终态
            else
                NF.insert(node);     // 非终态
        }
        P.push_back(F);
        P.push_back(NF);
        std::vector<std::set<Node>> W = {F, NF}; // 待分割集合

        // 主循环，不断细分划分集合
        while (!W.empty()) {
            W.erase(W.begin()); // 取出一个待分割集合
            std::set<Node> p_remove;
            for (const auto& tag : nfa.tags) { // 对每个输入符号
                std::vector<std::set<Node>> X;
                for (auto & it : P) { // 遍历每个划分
                    std::set<Node> p_tag, p_not_tag;
                    for (const auto& node : it) {
                        // 如果该节点有tag边，则加入p_tag
                        for (const auto& e : this->edge_list) {
                            if ((e.getFromNodeId() == node.getId() && e.getTag() == tag) || (e.getToNodeId() == node.getId() && e.getTag() == tag)) {
                                p_tag.insert(node);
                            }
                        }
                    }
                    // p_not_tag为未被tag分割的节点
                    std::set_difference(it.begin(), it.end(), p_tag.begin(), p_tag.end(), std::inserter(p_not_tag, p_not_tag.begin()));
                    // 如果该划分可以被tag分割，则细分
                    if (!p_tag.empty() && !p_not_tag.empty()) {
                        X.push_back(p_tag);
                        X.push_back(p_not_tag);
                        p_remove = it;
                        break;
                    }
                }
                // 用细分结果替换原划分
                auto pos = std::find(P.begin(), P.end(), p_remove);
                if (pos != P.end()) {
                    P.erase(pos);
                }
                for (const auto& x : X) {
                    P.push_back(x);
                    auto found = std::find(W.begin(), W.end(), x);
                    if (found == W.end())
                        W.push_back(x);
                    else
                        W.erase(found);
                }
            }
        }
        // 构造新的DFA节点和边
        std::vector<Node> new_node_list;
        std::vector<Edge> new_edge_list;
        for (int i = 0; i < P.size(); ++i) {
            const auto& p = P[i];
            Node first_in_set = *p.begin();
            // 新节点属性取该划分中第一个节点
            new_node_list.emplace_back(i, first_in_set.getIsLast(), first_in_set.getIsNeedRollback(), first_in_set.getType());

            // 重新建立边
            for (const auto& e : this->edge_list) {
                int from_node_id = 0, to_node_id = 0;
                for (int j = 0; j < P.size(); ++j) {
                    if (P[j].find(this->node_list[e.getFromNodeId()]) != P[j].end()) {
                        from_node_id = j;
                    }
                    if (P[j].find(this->node_list[e.getToNodeId()]) != P[j].end()) {
                        to_node_id = j;
                    }
                }
                new_edge_list.emplace_back(from_node_id, to_node_id, e.getTag());
            }
        }
        // 更新DFA
        this->node_list = new_node_list;
        this->edge_list = new_edge_list;
        this->start_id = 1;
        this->current_id = 1;
    }


    string toString()  {
        stringstream sb;
        sb << "DFA Node Amount: " << this->node_list.size() << std::endl;
        for (const auto& e : this->edge_list) {
            const Node& fromNode = this->node_list[e.getFromNodeId()];
            const Node& toNode = this->node_list[e.getToNodeId()];
            sb << "Node:" << e.getFromNodeId();
            sb << "(" << fromNode.getIsLast() << "," << fromNode.getIsNeedRollback() << "," << fromNode.getType();
            sb << ")----- " << e.getTag() << " ----->";
            sb << "Node:" << e.getToNodeId();
            sb << "(" << toNode.getIsLast() << "," << toNode.getIsNeedRollback() << "," << toNode.getType() << ")";
            sb << std::endl;
        }
        sb << "======================================================================" << std::endl;
        return sb.str();
    }


    bool isFinal(int id) const {
        return this->node_list[id].getIsLast();
    }

    bool needRollback(int id) const {
        return this->node_list[id].getIsNeedRollback();
    }

    std::string getType(int id) const {
        return this->node_list[id].getType();
    }

    bool getNextNode(const std::string& symbol) {
        for (const auto& edge : this->edge_list) {
            std::regex pattern(edge.getTag());
            if (edge.getFromNodeId() == this->current_id && std::regex_match(symbol, pattern)) {
                this->current_id = edge.getToNodeId();
                return true;
            }
        }
        return false;
    }

    std::string getTokenType(const std::string& token, const std::string& nodeType) {
        static const std::set<std::string> simpleTypes{"OP", "SE", "INT", "FLOAT"};
        if (simpleTypes.find(nodeType) != simpleTypes.end()) {
            return nodeType;
        } else if (nodeType == "IDN_KW_OP") {
            if (TYPE_TO_CONTENT_DICT_KW.find(token) != TYPE_TO_CONTENT_DICT_KW.end()) {
                return "KW";
            } else if (TYPE_TO_CONTENT_DICT_OP.find(token) != TYPE_TO_CONTENT_DICT_OP.end()) {
                return "OP";
            } else {
                return "IDN";
            }
        } else {
            return "ERROR";
        }
    }

    std::string getTokenId(const std::string& token, const std::string& tokenType) {
        if (tokenType == "IDN" || tokenType == "INT" || tokenType == "FLOAT") {
            return token;
        } else if (tokenType == "KW") {
            auto it = TYPE_TO_CONTENT_DICT_KW.find(token);
            if (it != TYPE_TO_CONTENT_DICT_KW.end()) {
                return std::to_string(it->second);
            }
        } else if (tokenType == "OP") {
            auto it = TYPE_TO_CONTENT_DICT_OP.find(token);
            if (it != TYPE_TO_CONTENT_DICT_OP.end()) {
                return std::to_string(it->second);
            }
        } else if (tokenType == "SE") {
            auto it = TYPE_TO_CONTENT_DICT_SE.find(token);
            if (it != TYPE_TO_CONTENT_DICT_SE.end()) {
                return std::to_string(it->second);
            }
        }
        return "error";
    }

    void getStart() {
        this->current_id = this->start_id;
    }
};

#endif //DFA_H