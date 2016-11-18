#include "kmeans.h"
#include <random>
#include <ctime>
#include <algorithm>
#include <limits>
#include <chrono>
struct comp {
	bool operator()(std::pair<int, double> t1, std::pair<int, double> t2) {
		return t1.second < t2.second;
	}
};
K_Means::Cluster K_Means::kmeans(int k)const {
	std::map<int, int> belongs;
	std::map<int, std::set<int>> clusters;
	bool isChanged = true;
	auto cens = randSelectNode(k);
	auto opt = [](double a, double b) {return std::max(a, b); };
	while (isChanged) {
		isChanged = false;
		for (auto&i : nodes_) {
			std::vector<std::pair<int, double>> temp;
			for (auto&k : cens) {
				temp.push_back(std::make_pair(k, distance(k, i)));
			}
			auto it = std::min_element(temp.begin(), temp.end(), comp());
			if (!isChanged && belongs[i] != it->first)
				isChanged = true;
			belongs[i] = it->first;
		}
		clusters.clear();
		for (auto&i : belongs) {
			clusters[i.second].insert(i.first);
		}
		for (auto&i : cens) {
			i = calCentroid(clusters[i], opt);
		}
	}
	Cluster ret;
	for (auto&i : clusters) {
		double res = 0.0;
		for (auto&k : i.second) {
			res = opt(res, distance(i.first, k));
		}
		ret[i.first] = std::make_pair(i.second, res);
	}
	return ret;
}

void K_Means::biKmeans(int k) {
	clusters_ = kmeans(1);
	while (clusters_.size() <= k) {
		oneMoreNode();
	}
}

void K_Means::oneMoreNode() {
	assert(!clusters_.empty());
	int id = -1;
	double error = DBL_MAX;
	Cluster t;
	for (auto&i : clusters_) {
		K_Means ck(i.second.first, mat_, idTable_);
		auto res = ck.kmeans(2);
		double bias = 0.0;
		for (auto&k : clusters_) {
			if (k.first == i.first) continue;
			bias += k.second.second;
		}
		for (auto&k : res)
			bias += k.second.second;
		if (bias < error) {
			error = bias;
			id = i.first;
			t = res;
		}
	}
	clusters_.erase(id);
	for (auto&i : t)
		clusters_.insert(i);
}

double K_Means::splitMaxCluster() {
	if (clusters_.empty()) {
		clusters_ = kmeans(1);
	}
	auto it = std::max_element(clusters_.begin(), clusters_.end(), clusterComp());
	K_Means k_means(it->second.first, mat_, idTable_);
	auto res = k_means.kmeans(2);
	clusters_.erase(it);
	for (auto&i : res)
		clusters_.insert(i);
	return std::max_element(clusters_.begin(), clusters_.end(), clusterComp())->second.second;
}

std::vector<int> K_Means::randSelectNode(int k)const {
	assert(k <= nodes_.size());
	std::vector<int> ids;
	for (auto&i : nodes_)
		ids.push_back(i);
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(ids.begin(), ids.end(), std::default_random_engine(seed));
	return std::vector<int>(ids.begin(), ids.begin() + k);
}

int K_Means::calCentroid(const std::set<int>& nodes,std::function<double(double,double)> opt)const {
	std::vector<std::pair<int, double>> sums;
	for (auto&i : nodes) {
		double res = 0.0;
		for (auto&k : nodes) {
			res = opt(res, distance(i, k));
		}
		sums.push_back(std::make_pair(i, res));
	}
	return std::min_element(sums.begin(), sums.end(), comp())->first;
}

