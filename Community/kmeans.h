#pragma once
#include <set>
#include <vector>
#include <assert.h>
#include <map>
#include <functional>
#include "graph.h"
struct clusterComp {
	bool operator()(const std::pair<int, std::pair<std::set<int>, double>>&a, const std::pair<int, std::pair<std::set<int>, double>>&b) {
		return std::less<double>()(a.second.second, b.second.second);
	}
};
class K_Means {
public:
	using Cluster = std::map<int, std::pair<std::set<int>, double>>;
	explicit K_Means(const std::set<int>& nodes, const Graph::matrix& mat, const std::map<int, int>& idTable) 
		:nodes_(nodes), mat_(mat), idTable_(idTable){}
	Cluster kmeans(int k)const;
	void biKmeans(int k);	
	void oneMoreNode();
	double splitMaxCluster();
	const Cluster& getClusets()const { return clusters_; }
private:
	std::vector<int> randSelectNode(int k)const;
	int id2index(int id) const{
		auto it = idTable_.find(id);
		assert(it != idTable_.end());
		return it->second;
	}
	double distance(int s, int t)const { return mat_[id2index(s)][id2index(t)]; }
	int calCentroid(const std::set<int>& nodes,std::function<double(double,double)>)const;
	const std::set<int>& nodes_;
	const Graph::matrix& mat_;
	const std::map<int, int>& idTable_;
	Cluster clusters_;
};
