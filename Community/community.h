#pragma once
#include <map>
#include <set>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include "graph.h"

struct Community {
	std::set<int> nodes;
	double r;
	int id;
	int leftId, rightId;
	int centraNode;
	Community(int ID) :id(ID), r(DBL_MAX), leftId(-1), rightId(-1), centraNode(-1) {}
};
struct pair_comp {
	bool operator()(std::pair<int, double> t1, std::pair<int, double> t2) {
		return t1.second < t2.second;
	}
};
struct community_comp{
	bool operator()(Community& c1, Community& c2) {
		return c1.r < c2.r;
	}
};
bool operator == (const Community& c1, const Community& c2);
class Solution {
public:
	using pos = std::pair<int, double>;
	Solution(const std::string& netfile, const std::string& communityfile);
	Graph generateTimeNet(Graph &g);
	std::vector<Community> generateCommunities(const std::string&);
	pos calculateRc(const Community & C);
	Community communityMerge(Community& c1, Community& c2);
	std::vector<int> closelyCommunities(int id);	//idÔÚpresentBBSÖÐµÄclosely connected communities

private:
	std::vector<Community> communities;		
	std::list<int> presentBBS;		
	std::list<int> unusedBBS;
	Graph originNet;
	Graph timeNet;
	Graph::matrix timeMat;
	std::map<int, int> idTable;
	bool iscloselyCommunities(int i, int j);
	int id2index(int id) {
		auto it = idTable.find(id);
		assert(it != idTable.end());
		return it->second;
	}
	pos maxVec(std::vector<double>::iterator b, std::vector<double>::iterator e) {
		auto it = std::max_element(b, e);
		return std::make_pair(std::distance(b, it), *it);
	}

};