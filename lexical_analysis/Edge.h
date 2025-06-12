#ifndef EDGE_H
#define EDGE_H

#include <string>

// Edge: 有向边定义，支持从节点、到节点及标签的访问
class Edge {
private:
    int from_node_id;       // 源节点 id
    int to_node_id;         // 目标节点 id
    std::string tag;        // 边的标记

public:
    Edge(int from_node_id, int to_node_id, const std::string &tag)
        : from_node_id(from_node_id), to_node_id(to_node_id), tag(tag) {}

    // 访问器
    int getFromNodeId() const { return from_node_id; }
    int getToNodeId()   const { return to_node_id;   }
    const std::string &getTag() const { return tag; }
};

#endif // EDGE_H