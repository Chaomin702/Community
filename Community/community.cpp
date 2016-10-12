#include "community.h"
#include <math.h>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <algorithm>
Solution::Solution(const std::string & netfile, const std::string & communityfile):originNet(netfile){
	timeNet = generateTimeNet(originNet);	
	communities = generateCommunities(communityfile);
	timeMat = timeNet.johnson();
	for (auto &i : communities)
		presentBBS.push_back(i.first);	
	auto L = timeNet.getNodeList();
	for (decltype(L.size()) i = 0; i < L.size(); ++i)
		idTable[L[i]] = i;	
	for (auto &i : communities) {
		auto t = calculateRc(communities[i.first]);
		i.second.centraNode = t.first;
		i.second.r = t.second;
	}
}
Graph Solution::generateTimeNet(Graph & g){
	Graph net;
	std::vector<double> degrees;
	auto L = g.getNodeList();
	for (auto i : L) {
		double d = 0.0;
		for (auto &k : g.getNeighbors(i)) {
			d += k.weight;
		}
		degrees.push_back(d);
		for (auto &k : g.getNeighbors(i)) {
			net.insert(k.source, k.dest, d / (k.weight*k.weight));
		}
	}
	return net;
}
std::map<int, Community> Solution::generateCommunities(const std::string &filename){
	std::ifstream f(filename);
	assert(f);
	std::map<int,Community> bbs;
	std::map<int, std::vector<int>> id2nodes;
	std::string line;
	while (std::getline(f, line)) {
		int n, id;
		std::stringstream ss(line);
		ss >> n >> id;
		id2nodes[id].push_back(n);
	}
	for (auto &i : id2nodes) {
		bbs[i.first] = Community(i.first);
		for (auto j : i.second)
			bbs[i.first].nodes.insert(j);
	}
	return bbs;
}

 Solution::pos Solution::calculateRc(const Community& C){
	std::vector<int> ids, indexs;
	for (auto &i : C.nodes) {
		ids.push_back(i);
		indexs.push_back(id2index(i));
	}
	
	std::vector<std::pair<int, double>> resVec;
	//计算指定行的最大值
	for (auto i : ids) {
		int index = id2index(i);
		resVec.push_back(optVec(timeMat[index], indexs, std::less<double>()));
	}
	auto p = std::min_element(resVec.begin(), resVec.end(), pair_comp());
	return std::make_pair(ids[std::distance(resVec.begin(),p)], p->second);
}

 Community Solution::communityMerge(Community & c1, Community & c2){
	 Community res((--communities.end())->first + 1);
	 res.nodes.insert(c1.nodes.begin(), c1.nodes.end());
	 res.nodes.insert(c2.nodes.begin(), c2.nodes.end());
	 res.leftId = c1.id;
	 res.rightId = c2.id;
	 auto t = calculateRc(res);
	 res.r = t.second;
	 res.centraNode = t.first;
	 return res;
 }

 std::vector<int> Solution::closelyCommunities(int id){
	 std::vector<int> res;
	 for (auto i : presentBBS) {
		 if (iscloselyCommunities(id, i))
			 res.push_back(i);
	 }
	 return res;
 }

 Solution::pos Solution::minRc(std::list<int>& L){
	 std::vector<pos> rcs;
	 for (auto i : L) {
		 rcs.push_back(std::make_pair(i, communities[i].r));
	 }
	 auto it = std::min_element(rcs.begin(), rcs.end(), pair_comp());
	 return *it;
 }

 Solution::pos Solution::maxRc(std::list<int>& L){
	 std::vector<pos> rcs;
	 for (auto i : L) {
		 rcs.push_back(std::make_pair(i, communities[i].r));
	 }
	 auto it = std::max_element(rcs.begin(), rcs.end(), pair_comp());
	 return *it;
 }

 int Solution::mergeProcess(int k){
	 while (presentBBS.size() > k) {
		 std::list<int> bbs = presentBBS;
		 Community minBBS(-1);
		 while (!bbs.empty()) {
			 auto ci = minRc(bbs);
			 auto closelys = closelyCommunities(ci.first);
			 auto candidate = lowestConnectCommunities(ci.first, closelys);
			 auto ca = maxRc(presentBBS);	//存疑 参数是presentBBS 还是 bbs
			 if (candidate.r < ca.second) {	//理想情况
				 minBBS = candidate;
				 break;
			 }
			 else {
				 if (candidate.r < minBBS.r)
					 minBBS = candidate;
				 bbs.erase(std::find(bbs.begin(), bbs.end(), ci.first));
			 }
		 }
		 communities[minBBS.id] = minBBS;
		 presentBBS.erase(std::find(presentBBS.begin(), presentBBS.end(), minBBS.leftId));
		 presentBBS.erase(std::find(presentBBS.begin(), presentBBS.end(), minBBS.rightId));
		 presentBBS.push_back(minBBS.id);
	 }
	 return presentBBS.size();
 }

 Community Solution::lowestConnectCommunities(int id, std::vector<int>& nbr){
	 std::vector<Community> cs;
	 for (auto i : nbr) {
		 cs.push_back(communityMerge(communities[id], communities[i]));
	 }
	 auto it = std::min_element(cs.begin(), cs.end(), community_comp());
	 return *it;
 }

 bool Solution::iscloselyCommunities(int i, int j){
	 assert(isCommunityExist(i) && isCommunityExist(j));
	 if (i == j) return false;
	 double w1 = 0.0, w2 = 0.0;
	 for (auto &n : communities[i].nodes) {
		 auto nbs = originNet.getNeighbors(n);
		 for (auto &e : nbs) {
			 if (communities[j].nodes.find(e.dest) != communities[j].nodes.end())
				 w1 += e.weight;
			 if (communities[i].nodes.find(e.dest) == communities[i].nodes.end())
				 w2 += e.weight;
		 }
	 }
	 return (w1 / communities[j].nodes.size()) >= (w2 / (originNet.nodesNum() - communities[i].nodes.size()));
 }
Solution::pos Solution::optVec(std::vector<double>&v, std::vector<int>&p, std::function<bool(double, double)> opt){
	assert(!p.empty() && !v.empty());
	pos t(p[0], v[p[0]]);
	for (auto &i : p) {
		if (opt(t.second, v[i])) {
			t.first = i;
			t.second = v[i];
		}
	}
	return t;
 }
 bool operator == (const Community& c1, const Community& c2) {
	 return c1.id == c2.id;
 }

 std::ostream & operator<<(std::ostream &os, Solution &so){
	 os << "communities size: " << so.presentBBS.size() << "\n";
	 os << "communities id    centre node    diffusion time" << "\n";
	 for (auto &i : so.communities) {
		 os << i.first << "   " << i.second.centraNode << "    " << i.second.r << "\n";
	 }
	 return os;
 }
