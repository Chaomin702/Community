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
#include <boost/heap/fibonacci_heap.hpp>
void Graph::insert(const Node &n) {
	if (isNodeExist(n.id))
		return;
	nodes[n.id] = n;
}

void Graph::insert(const Edge &e) {
	if (!isNodeExist(e.source))
		insert(Node(e.source));
	if (!isNodeExist(e.dest))
		insert(Node(e.dest));
	neighbors[e.source].push_back(e);
	++getNodeRef(e.source).outdegree;
	++getNodeRef(e.dest).indegree;
}

bool Graph::relax(int u, int v, double w) {
	if (getNodeRef(v).priority > getNodeRef(u).priority + w) {
		getNodeRef(v).priority = getNodeRef(u).priority + w;
		getNodeRef(v).parent = u;
		return true;
	}
	return false;
}

Graph::Graph(const std::string &filename) {
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

double Graph::weight(int s, int t) {
	assert(isNodeExist(s) && s != t);
	for (auto &i : neighbors[s]) {
		if (i.dest == t)
			return i.weight;
	}
	return DBL_MAX;
}

void Graph::reset() {
	for (auto &i : nodes) {
		i.second.parent = NIL;
		i.second.priority = DBL_MAX;
		i.second.type = UNDISCOVERED;
	}
}

void Graph::dijkstra(int s) {
	reset();
	getNodeRef(s).priority = 0;
	boost::heap::fibonacci_heap<Node, boost::heap::compare<compare_node>> Q;
	std::map<int, boost::heap::fibonacci_heap<Node, boost::heap::compare<compare_node>>::handle_type> ptrs;
	for (auto &i : nodes) {
		ptrs[i.first] = Q.push(getNodeRef(i.first));
	}
	while (!Q.empty()) {
		auto n = Q.top();
		int u = n.id;
		for (auto &i : neighbors[u]) {
			if ((UNDISCOVERED == getNodeRef(u).type) && relax(u, i.dest, i.weight)) {
				Q.update(ptrs[i.dest], getNodeRef(i.dest));
			}
		}
		getNodeRef(u).type = VISITED;
		Q.pop();
	}
}

std::list<int> Graph::shortestpath(int s, int t) {
	std::list<int> L;
	int parent = t;
	while (parent != NIL) {
		L.push_front(parent);
		parent = getNodeRef(parent).parent;
	}
	return L;
}

Graph::matrix Graph::johnson() {
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
bool operator<=(const Node & m, const Node & n) {
	return m.priority <= n.priority;
}
bool operator>=(const Node & m, const Node & n) {
	return m.priority >= n.priority;
}
bool operator > (const Node& m, const Node& n) {
	return m.priority > n.priority;
}
bool operator==(const Node & m, const Node & n) {
	return m.id == n.id;
}
std::ostream & operator<<(std::ostream & os, const Node & m) {
	os << m.id << " ";
	return os;
}
bool operator == (const Edge& m, const Edge&n) {
	return m.source == n.source &&
		m.dest == n.dest &&
		m.weight == n.weight;
}
void Graph::exportGraph(const std::string & name) {
	std::ofstream of(name, std::ofstream::trunc);
	of << "Source,Target,Type,Id,Weight\n";
	int c = 1;
	for (auto &i : neighbors) {
		for (auto &k : i.second) {
			of << k.source << "," << k.dest << ",Undirected," << c++ << "," << k.weight << "\n";
		}
	}
	of.close();
}