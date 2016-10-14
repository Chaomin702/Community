#include "community.h"
#include <math.h>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
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

void Solution::reset(const std::string & communityfile){
	communities = generateCommunities(communityfile);
	presentBBS.clear();
	for (auto &i : communities)
		presentBBS.push_back(i.first);
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
#ifdef PRINT
	 int cnt = 1;
	 std::cout << "\nmerging process...\n" << std::endl;
#endif
	 while (1) {
		 std::list<int> bbs = presentBBS;
		 Community minBBS(-1);
		 auto ca = maxRc(presentBBS);	//存疑 参数是presentBBS 还是 bbs
		 while (!bbs.empty()) {
			 auto ci = minRc(bbs);
			 auto closelys = closelyCommunities(ci.first);
#ifdef PRINT
			 std::cout << '\n' << cnt++ << "." << '\n';
			 std::cout << "min R(C) id: " << ci.first << "   neightbors: ";
			 for (auto &i : closelys)
				 std::cout << i <<"  ";
			 std::cout << "\n";
#endif
			 if (closelys.empty()) {
				 bbs.erase(std::find(bbs.begin(), bbs.end(), ci.first));
				 continue;
			 }
			 auto candidate = lowestConnectCommunities(ci.first, closelys);
			 if (candidate.r <= ca.second) {	//理想情况
				 minBBS = candidate;
				 break;
			 }
			 else {
				 if (candidate.r < minBBS.r)
					 minBBS = candidate;
				 bbs.erase(std::find(bbs.begin(), bbs.end(), ci.first));
			 }
		 }
		 if (minBBS.r <= ca.second || presentBBS.size() > k) {
			 updateMergeCommunities(minBBS);
#ifdef PRINT
			 std::cout << minBBS.leftId << "+" << minBBS.rightId << "->" << minBBS.id << std::endl;
			 std::cout << *this;
#endif
		 }
		 else {
			 break;		//无可合并社区
		 }
	 }
	return presentBBS.size();
 }

 Community Solution::lowestConnectCommunities(int id, std::vector<int>& nbr){
	 assert(!nbr.empty());
	 std::vector<Community> cs;
	 for (auto i : nbr) {
		 cs.push_back(communityMerge(communities[id], communities[i]));
	 }
	 auto it = std::min_element(cs.begin(), cs.end(), community_comp());
	 return *it;
 }

 void Solution::updateMergeCommunities(Community &C){
	 communities[C.id] = C;
	 presentBBS.erase(std::find(presentBBS.begin(), presentBBS.end(), C.leftId));
	 presentBBS.erase(std::find(presentBBS.begin(), presentBBS.end(), C.rightId));
	 presentBBS.push_back(C.id);
 }

 void Solution::updateSplitCommunities(int id){
	 assert(std::find(presentBBS.begin(), presentBBS.end(), id) != presentBBS.end());
	 presentBBS.erase(std::find(presentBBS.begin(), presentBBS.end(), id));
	 presentBBS.push_back(communities[id].leftId);
	 presentBBS.push_back(communities[id].rightId);
 }

double Solution::optDuffusionTime(std::vector<int>& duffsionSet, std::set<int>& other, std::function<double(double, double)> opt){
	std::vector<int> otherindex;
	std::vector<std::vector<double>::const_iterator> its;
	for (auto i : duffsionSet)
		its.push_back(timeMat[id2index(i)].cbegin());
	for (auto i : other)
		otherindex.push_back(id2index(i));
	for (auto i = otherindex.size()-1; i > 0; --i)
		otherindex[i] = otherindex[i] - otherindex[i - 1];
	auto cmp = [](std::vector<double>::const_iterator i1, std::vector<double>::const_iterator i2) {
		return *i1 < *i2;
	};
	double res = 0.0;
	for (auto i : otherindex) {
		for (auto &t : its)
			std::advance(t, i);
		auto it = std::min_element(its.begin(), its.end(), cmp);
		res = opt(res, **it);
	}
	return res;
 }
double Solution::updateRc(Community & C){
	auto opt = [](double a, double b) {return std::max(a, b); };
	auto t =  optDuffusionTime(C.diffusionNodes, C.nodes, opt);
	return t;
}
Solution::pos Solution::selectDuffusionNode(Community & C){
	std::vector<pos> res;
	for (auto i : C.nodes) {
		auto temp = C.diffusionNodes;
		temp.push_back(i);
		auto d = optDuffusionTime(temp, C.nodes,std::plus<double>());
		res.push_back(std::make_pair(i, d));
	}
	auto it = std::min_element(res.begin(), res.end(), pair_comp());
	return *it;
}

double Solution::restoringProcess(int k){
#ifdef PRINT
	std::cout << "\nrestoring process...\n" << std::endl;
#endif
	assert(k >= presentBBS.size());
	for (auto &i : communities)
		i.second.diffusionNodes.push_back(i.second.centraNode);
	int s = k - presentBBS.size();
	while (s--) {
		auto ca = maxRc(presentBBS);
		if (isMergedCommunity(ca.first)) {
#ifdef PRINT
			std::cout << "\n" << ca.first << "->" << communities[ca.first].leftId << "+" << communities[ca.first].rightId << "\n";
#endif
			updateSplitCommunities(ca.first);
		}
		else {
			if (communities[ca.first].diffusionNodes.size() == 1) {
				communities[ca.first].diffusionNodes.pop_back();
				auto t = selectDuffusionNode(communities[ca.first]);
				communities[ca.first].diffusionNodes.push_back(t.first);
			}
			auto t = selectDuffusionNode(communities[ca.first]);
			communities[ca.first].diffusionNodes.push_back(t.first);
			communities[ca.first].r = updateRc(communities[ca.first]);
		}
		std::cout << *this;
	}
	return maxRc(presentBBS).second;
}

double Solution::communityBaseAlgorithm(int k){
	mergeProcess(k);
	return restoringProcess(k);
}

std::vector<int> Solution::diffusionNodes(){
	std::vector<int> res;
	for (auto i : presentBBS) {
		for (auto k : communities[i].diffusionNodes)
			res.push_back(k);
	}
	return res;
}

void Solution::exportNet(const std::string &name){
	timeNet.exportGraph("edge" + name);
	exportNodes("nodes" + name);
}

std::vector<int> Solution::nodeBelongTo(int id){
	std::vector<int> res;
	for (auto i : presentBBS) {
		if (communities[i].nodes.find(id) != communities[i].nodes.end())
			res.push_back(i);
	}
	return res;
}

Solution::pos Solution::closeness(int id){
	int index = id2index(id);
	double sum = 0.0;
	for (auto i : timeMat[index])
		sum += i;
	return std::make_pair(id, 1.0 / sum);
}

double Solution::naiveAlgorithm(int k){
	std::vector<pos> res;
	std::set<int> other;
	for (auto i : idTable) {
		res.push_back(closeness(i.first));
		other.insert(i.first);
	}
	std::sort(res.begin(), res.end(), pair_comp());
	std::vector<int> ids;
	for (auto it = res.rbegin(); ids.size() < k; ++it)
		ids.push_back(it->first);
	auto opt = [](double a, double b) {return std::max(a, b); };
	auto t = optDuffusionTime(ids, other, opt);
	return t;
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
void Solution::exportNodes(const std::string & name){
	std::ofstream of(name,std::ofstream::trunc);
	struct style {
		int id; int cy; double cs; double r;
	};
	std::vector<style> res;
	std::vector<double> rs;
	of << "Id,Community,Closeness,Radius\n";
	auto opt = [](double a, double b) {return std::max(a, b); };
	for (auto &i : communities) {
		for (auto &k : i.second.nodes) {
			auto c = closeness(k);
			auto r = optDuffusionTime(std::vector<int>(1,k), i.second.nodes, opt);
			rs.push_back(1.0 / r);
			res.push_back({ k,i.first,c.second,0.0 });
			//of << k << "," << i.first << "," << c.second <<"," << 1.0/r <<"\n";
		}
		auto ma = std::max_element(rs.begin(), rs.end());
		auto mi = std::min_element(rs.begin(), rs.end());
		double m = *ma;
		double n = *mi;
		for (auto &i : rs) {
			i = (i - n) / (m - n);
		}
		for (size_t i = 0; i < rs.size(); ++i) {
			res[i].r = rs[i];
		}
		for (auto &i : res)
			of << i.id << "," << i.cy << "," << i.cs << "," << i.r << "\n";
		res.clear();
		rs.clear();
	}
	of.close();
}
 bool operator == (const Community& c1, const Community& c2) {
	 return c1.id == c2.id;
 }

 std::ostream & operator<<(std::ostream &os, Solution &so){
	 os << "communities size: " << so.presentBBS.size() << "\n";
	 os << "id  centre  time  diffusion-nodes" << "\n";
	 for (auto &i : so.presentBBS) {
		 assert(i == so.communities[i].id);
		 os << std::left <<std::setw(5)<< i << std::setw(5) << so.communities[i].centraNode  << std::setw(5) << std::setprecision(4) << so.communities[i].r<<"    ";
		 if (!so.communities[i].diffusionNodes.empty())
			 for (auto k : so.communities[i].diffusionNodes)
				 os << k << " ";
		 os << "\n";
	 }
	 return os;
 }
