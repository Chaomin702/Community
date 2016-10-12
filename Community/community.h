#pragma once
#include <map>
#include <set>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include "graph.h"

struct Community {
	std::set<int> nodes;
	double r;
	int id;
	int leftId, rightId;
	int centraNode;
	std::vector<int> diffusionNodes;
	explicit Community(int ID) :id(ID), r(DBL_MAX), leftId(-1), rightId(-1), centraNode(-1) {}
	Community() :id(-1), r(DBL_MAX), leftId(-1), rightId(-1), centraNode(-1) {}
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
	friend std::ostream& operator << (std::ostream&, Solution&);

	Solution(const std::string& netfile, const std::string& communityfile);
	Graph generateTimeNet(Graph &g);
	std::map<int, Community> generateCommunities(const std::string&);
	pos calculateRc(const Community & C);
	double updateRc(Community& C);
	Community communityMerge(Community& c1, Community& c2);
	std::vector<int> closelyCommunities(int id);	//id在presentBBS中的closely connected communities
	pos minRc(std::list<int>& L);
	pos maxRc(std::list<int>& L);
	int mergeProcess();
	Community lowestConnectCommunities(int id, std::vector<int>& nbr);	
	void updateMergeCommunities(Community&);
	void updateSplitCommunities(int id);
	//在社区内,求取指定的id集的扩散节点
	double optDuffusionTime(std::vector<int>& duffsionSet, std::set<int>& other, std::function<double(double,double)>);
	pos selectDuffusionNode(Community& C);
	double restoringProcess(int k);
	bool isMergedCommunity(int id) {
		assert(isCommunityExist(id));
		return communities[id].leftId >= 0 && communities[id].rightId >= 0;
	}
	double communityBaseAlgorithm(int k);
	std::vector<int> diffusionNodes();
private:
	std::map<int,Community> communities;		
	std::list<int> presentBBS;		
	std::list<int> unusedBBS;
	Graph originNet;
	Graph timeNet;
	Graph::matrix timeMat;
	std::map<int, int> idTable;
	bool isCommunityExist(int id) {
		return communities.find(id) != communities.end();
	}
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
	//对vector的指定元素列表做比较操作 
	pos optVec(std::vector<double>&, std::vector<int>&, std::function<bool(double, double)>);
};
std::ostream& operator << (std::ostream&, Solution&);