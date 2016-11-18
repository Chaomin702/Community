#pragma once
#include <vector>
#include <unordered_map>
#include <limits>
#include <assert.h>
#include <float.h>
#include <list>
#include <map>
#include <unordered_set>
enum {
	NIL = -1
};
typedef enum {
	UNDISCOVERED,
	VISITED
}nodeType;
struct Node {
	int id;		
	int indegree, outdegree;
	int parent;
	double priority;		//路径权重
	nodeType type;
	Node() = default;
	explicit Node(int ID) :id(ID), indegree(0), outdegree(0), parent(NIL), priority(DBL_MAX),type(UNDISCOVERED) {}
};
bool operator < (const Node& m, const Node& n);
bool operator <=(const Node&m, const Node&n);
bool operator >=(const Node&m, const Node&n);
bool operator > (const Node& m, const Node& n);
bool operator ==(const Node&m, const Node&n);
struct compare_node
{
	bool operator()(const Node& n1, const Node& n2) const {
		return n1.priority > n2.priority;
	}
};
std::ostream& operator <<(std::ostream& os, const Node&m);
struct Edge {
	int source, dest;
	double weight;
	Edge(int s, int d, double w) :source(s), dest(d), weight(w) {}
};
bool operator == (const Edge& m, const Edge&n);
class Graph {
public:
	Graph() = default;
	//从文件导入图，格式： node_source node_dest edge_weight /r/t ...
	Graph(const std::string&);
	using matrix = std::vector<std::vector<double>>;
	//节点数量
	int nodesNum()const { return nodes.size(); }
	//插入节点
	void insert(int id) {
		insert(Node(id));
	}
	//s->t的边权重，不存在则返回DBL_MAX
	double weight(int s, int t);
	//插入边(source,dest,weight)
	void insert(int s, int t, double w) {
		insert(Edge(s, t, w));
	}
	//获取节点拷贝
	Node getNode(int id) const{
		auto it = nodes.find(id);
		assert(it != nodes.end());
		return it->second;
	}
	//节点近邻
	const std::vector<Edge>& getNeighbors(int id)const{
		auto it = neighbors.find(id);
		assert(it != neighbors.end());
		return it->second;
	}
	//节点列表
	std::vector<int> getNodeList()const {
		std::vector<int> L;
		for (auto &i : nodes)
			L.push_back(i.first);
		return L;
	}
	//获取节点权重
	double nodePriority(int id)const{return getNodeRef(id).priority; }
	//借点前驱
	int nodeParent(int id)const {
		return getNodeRef(id).parent;
	}
	void reset();
	//dijkstra最短路方法，参照CLRS chapter24实现
	void dijkstra(int s);
	//获取s->t最短路径链表
	std::list<int> shortestpath(int s, int t);
	//计算所有节点对的最短路径权重
	matrix johnson();
	//导出为csv文件
	void exportGraph(const std::string&);
private:
	std::map<int, Node> nodes;
	std::map<int, std::vector<Edge>> neighbors;
	bool isNodeExist(int id)const {
		return nodes.end() != nodes.find(id);
	}
	const Node& getNodeRef(int id) const{
		assert(isNodeExist(id));
		return nodes.find(id)->second;
	}
	Node& getNodeRef(int id) {
		assert(isNodeExist(id));
		return nodes[id];
	}
	void insert(const Node&);
	void insert(const Edge&);
	//松弛方法，用于dijkstra
	bool relax(int u, int v, double w);

};