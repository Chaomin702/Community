#ifdef _WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "graph.h"


void Graph::insert(const Node &n){
	if (isNodeExist(n.id))
		return;
	nodes[n.id] = n;
}

void Graph::insert(const Edge &e){
	if (!isNodeExist(e.source))
		insert(Node(e.source));
	if (!isNodeExist(e.dest))
		insert(Node(e.dest));
	neighbors[e.source].push_back(e);
	++getNodeRef(e.source).outdegree;
	++getNodeRef(e.dest).indegree;
}

bool Graph::relax(int u, int v, double w){
	if (getNodeRef(v).priority > getNodeRef(u).priority + w) {
		getNodeRef(v).priority = getNodeRef(u).priority + w;
		getNodeRef(v).parent = u;
		return true;
	}
	return false;
}

Graph::Graph(const std::string &filename){
	std::ifstream f(filename);
	assert(f);
	std::string line;
	int source, dest;
	double weight;
	while (std::getline(f, line)) {
		std::stringstream ss(line);
		ss >> source >> dest >> weight;
		insert(source, dest, weight);
	}
	f.close();
}

double Graph::weight(int s, int t){
	assert(isNodeExist(s) && s!=t);
	for (auto &i : neighbors[s]) {
		if (i.dest == t)
			return i.weight;
	}
	return DBL_MAX;
}

void Graph::reset(){
	for (auto &i : nodes) {
		i.second.parent = NIL;
		i.second.priority = DBL_MAX;
		i.second.type = UNDISCOVERED;
	}
}

void Graph::dijkstra(int s){
	using fibNode = fib::node<Node>;	//fibonacci堆
	reset();
	getNodeRef(s).priority = 0;

	fib::FibonacciHeap<Node> Q;
	std::map<int, fibNode*> ptrs;//保存fib堆中的节点指针，方便decreaseKey传参
	for (auto &i : nodes) {
		ptrs[i.first] = Q.insert(getNodeRef(i.first));
	}

	while (!Q.isEmpty()) {
		Node n = Q.getMinimum();
		Q.removeMinimum();
		int u = n.id;
		for (auto &i : neighbors[u]) {
			if ((UNDISCOVERED == getNodeRef(u).type) && relax(n.id, i.dest, i.weight)) {
				Q.decreaseKey(ptrs[i.dest], getNodeRef(i.dest));
			}
		}
		getNodeRef(u).type = VISITED;
	}
}

std::list<int> Graph::shortestpath(int s, int t){
	std::list<int> L;
	int parent = t;
	while (parent != NIL) {
		L.push_front(parent);
		parent = getNodeRef(parent).parent;
	}
	return L;
}

Graph::matrix Graph::johnson(){
	matrix M;
	for (auto &i : nodes) {
		dijkstra(i.first);
		std::vector<double> temp;
		for (auto &k : nodes)
			temp.push_back(getNodeRef(k.first).priority);
		M.push_back(temp);
	}
	return M;
}

bool operator < (const Node& m, const Node& n) {
	return m.priority < n.priority;
}
bool operator > (const Node& m, const Node& n) {
	return m.priority > n.priority;
}
bool operator == (const Edge& m, const Edge&n) {
	return m.source == n.source &&
		m.dest == n.dest &&
		m.weight == n.weight;
}