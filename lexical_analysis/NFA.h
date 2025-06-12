#ifndef NFA_H
#define NFA_H

#include <vector>
#include <Node.h>
#include <Edge.h>
#include <string>
#include <sstream>

using namespace std;

class NFA {
public:
    vector<Node> node_list;
    vector<Edge> edge_list;
    vector<string> tags = {
        "[+]", "-", "[*]", "/", "%", "=", "[(]", "[)]", "[{]", "[}]", ";", ",",
        ">", "[^=]", "<", "[|]", "&", "[_a-zA-Z]", "[_0-9a-zA-Z]", "[^_0-9a-zA-Z]",
        "[1-9]", "[0-9]", "[^0-9]", "!", "[^0-9.]", "[.]"
    };

    NFA() {
        // 空转移，作为起始状态
        node_list.emplace_back(0, false, false, "");
        edge_list.emplace_back(0, 0, "epsilon");

        // 添加操作符 +
        node_list.emplace_back(1, true, false, "OP");
        edge_list.emplace_back(0, 1, "[+]");

        // 添加操作符 -
        node_list.emplace_back(2, true, false, "OP");
        edge_list.emplace_back(0, 2, "-");

        // 添加操作符 *
        node_list.emplace_back(3, true, false, "OP");
        edge_list.emplace_back(0, 3, "[*]");
    
        // 添加操作符 /
        node_list.emplace_back(4, true, false, "OP");
        edge_list.emplace_back(0, 4, "/");

        // 添加操作符 %
        node_list.emplace_back(5, true, false, "OP");
        edge_list.emplace_back(0, 5, "%");

        // 添加等号 =
        node_list.emplace_back(6, false, false, "");
        edge_list.emplace_back(0, 6, "=");

        // 添加左括号 (
        node_list.emplace_back(7, true, false, "SE");
        edge_list.emplace_back(0, 7, "[(]");

        // 添加右括号 )
        node_list.emplace_back(8, true, false, "SE");
        edge_list.emplace_back(0, 8, "[)]");

        // 添加左花括号 {
        node_list.emplace_back(9, true, false, "SE");
        edge_list.emplace_back(0, 9, "[{]");

        // 添加右花括号 }
        node_list.emplace_back(10, true, false, "SE");
        edge_list.emplace_back(0, 10, "[}]");

        // 添加分号 ;
        node_list.emplace_back(11, true, false, "SE");
        edge_list.emplace_back(0, 11, ";");

        // 添加逗号 ,
        node_list.emplace_back(12, true, false, "SE");
        edge_list.emplace_back(0, 12, ",");

        // 添加双字符操作符 >, <, |, &
        node_list.emplace_back(13, false, false, "");
        edge_list.emplace_back(0, 13, ">");
        node_list.emplace_back(14, true, false, "OP");
        edge_list.emplace_back(13, 14, "=");
        node_list.emplace_back(15, true, true, "OP");
        edge_list.emplace_back(13, 15, "[^=]");

        node_list.emplace_back(16, false, false, "");
        edge_list.emplace_back(0, 16, "<");
        node_list.emplace_back(17, true, false, "OP");
        edge_list.emplace_back(16, 17, "=");
        node_list.emplace_back(18, true, true, "OP");
        edge_list.emplace_back(16, 18, "[^=]");

        node_list.emplace_back(19, false, false, "");
        edge_list.emplace_back(0, 19, "[|]");
        node_list.emplace_back(20, true, false, "OP");
        edge_list.emplace_back(19, 20, "[|]");

        node_list.emplace_back(21, false, false, "");
        edge_list.emplace_back(0, 21, "&");
        node_list.emplace_back(22, true, false, "OP");
        edge_list.emplace_back(21, 22, "&");

        // 添加标识符和关键字
        node_list.emplace_back(23, false, false, "");
        node_list.emplace_back(24, true, true, "IDN_KW_OP");
        edge_list.emplace_back(0, 23, "[_a-zA-Z]");
        edge_list.emplace_back(23, 23, "[_0-9a-zA-Z]");
        edge_list.emplace_back(23, 24, "[^_0-9a-zA-Z]");

        // 添加整数
        node_list.emplace_back(25, false, false, "");
        edge_list.emplace_back(0, 25, "[0-9]");
        edge_list.emplace_back(25, 25, "[0-9]");
        node_list.emplace_back(26, true, true, "INT");
        edge_list.emplace_back(25, 26, "[^0-9.]");

        // 添加条件运算符 !
        node_list.emplace_back(27, false, false, "");
        edge_list.emplace_back(0, 27, "!");
        node_list.emplace_back(28, true, false, "OP");
        edge_list.emplace_back(27, 28, "=");
        node_list.emplace_back(29,true,true,"OP");
        edge_list.emplace_back(27,29,"[^=]");

        node_list.emplace_back(30, true, false, "OP");
        edge_list.emplace_back(6, 30, "=");
        node_list.emplace_back(31, true, true, "OP");
        edge_list.emplace_back(6, 31, "[^=]");

        // 添加浮点数
        node_list.emplace_back(32, false, false, "");
        edge_list.emplace_back(25, 32, "[.]");
        edge_list.emplace_back(32, 32, "[0-9]");
        node_list.emplace_back(33, true, true, "FLOAT");
        edge_list.emplace_back(32, 33, "[^0-9]");
    }

    string toString() {
        stringstream sb;
        sb << "NFA Node Amount: " << node_list.size() << std::endl;
        for (const auto& e : edge_list) {
            const Node& fromNode = node_list[e.getFromNodeId()];
            const Node& toNode = node_list[e.getToNodeId()];
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

};

#endif //NFA_H
