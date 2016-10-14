#ifdef _WINDOWS
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <list>
#include "graph.h"
#include "community.h"
static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")
//Í¼½Úµã¶È²âÊÔ
static void test_graph_degree() {
	Graph g("eg.txt");
	EXPECT_EQ_INT(1, g.getNode(0).indegree);
	EXPECT_EQ_INT(2, g.getNode(1).indegree);
	EXPECT_EQ_INT(2, g.getNode(2).indegree);
	EXPECT_EQ_INT(3, g.getNode(3).indegree);
	EXPECT_EQ_INT(2, g.getNode(4).indegree);

	EXPECT_EQ_INT(2, g.getNode(0).outdegree);
	EXPECT_EQ_INT(2, g.getNode(1).outdegree);
	EXPECT_EQ_INT(3, g.getNode(2).outdegree);
	EXPECT_EQ_INT(1, g.getNode(3).outdegree);
	EXPECT_EQ_INT(2, g.getNode(4).outdegree);
}
static void test_graph_weight() {
	Graph g("eg.txt");
	EXPECT_EQ_DOUBLE(10.0, g.weight(0, 1));
	EXPECT_EQ_DOUBLE(5.0, g.weight(0, 2));
	EXPECT_EQ_DOUBLE(2.0, g.weight(1, 2));
	EXPECT_EQ_DOUBLE(1.0, g.weight(1, 3));
	EXPECT_EQ_DOUBLE(3.0, g.weight(2, 1));
	EXPECT_EQ_DOUBLE(2.0, g.weight(2, 4));
	EXPECT_EQ_DOUBLE(4.0, g.weight(3, 4));
	EXPECT_EQ_DOUBLE(7.0, g.weight(4, 0));
	EXPECT_EQ_DOUBLE(6.0, g.weight(4, 3));
}
static void test_graph_dijkstra() {
	Graph g("eg.txt");
	g.dijkstra(0);
	std::list<int> ans = { 0,2,1 };
	std::list<int> res = g.shortestpath(0, 1);
	EXPECT_EQ_INT(ans.size(), res.size());
	auto it1 = ans.begin(), it2 = res.begin();
	while (it1 != ans.end()) {
		EXPECT_EQ_INT(*it1, *it2);
		++it1;
		++it2;
	}
	EXPECT_EQ_DOUBLE(8.0, g.nodePriority(1));
}
static void test_graph_johnson() {
	{
		Graph g("eg.txt");
		Graph::matrix m1 = g.johnson();
		Graph::matrix m2 = { {0,8,5,9,7},{11,0,2,1,4},{9,3,0,4,2},{11,19,16,0,4},{7,15,12,6,0} };
		EXPECT_EQ_INT(m1.size(), m2.size());
		for (Graph::matrix::size_type i = 0; i < m2.size(); ++i) {
			for (decltype(i) j = 0; j < m2[i].size(); ++j) {
				EXPECT_EQ_DOUBLE(m1[i][j], m2[i][j]);
			}
		}
	}
	{
		Graph g("eg2.txt");
		Graph::matrix m1 = g.johnson();
		Graph::matrix m2 = { { 0,8,5,9,7 },{ 11,0,2,1,4 },{ 9,3,0,4,2 },{ 11,19,16,0,4 },{ 7,15,12,6,0 } };
		EXPECT_EQ_INT(m1.size(), m2.size());
		for (Graph::matrix::size_type i = 0; i < m2.size(); ++i) {
			for (decltype(i) j = 0; j < m2[i].size(); ++j) {
				EXPECT_EQ_DOUBLE(m1[i][j], m2[i][j]);
			}
		}
	}
}
static void test_graph() {
	test_graph_degree();
	test_graph_weight();
	test_graph_dijkstra();
	test_graph_johnson();
}
static void test_community(int k) {
	Solution task("network.dat", "community.dat");
	task.exportNet("timeNet.csv");
	std::cout << task;
	auto t = task.communityBaseAlgorithm(k);
	std::cout << "min diffusion time: " << t << "\n";
	auto res = task.diffusionNodes();
	std::cout << "result list: (community id: node id)\n";
	for (auto i : res) {
		auto bt = task.nodeBelongTo(i);
		for (auto j : bt)
			std::cout << j << " ";
		std::cout << ": ";
		std::cout << i << " \n";
	}
	auto nt = task.naiveAlgorithm(k);
	std::cout << "naive algorithm result: " << nt << std::endl;
}
std::vector<std::pair<double, double>> testAlgorithm() {
	Solution task("network.dat", "community.dat");
	int m = task.nodesNum() / 20;
	std::vector<std::pair<double, double>> res;
	for (int i = 1; i <= m; ++i) {
		auto ct = task.communityBaseAlgorithm(i);
		auto nt = task.naiveAlgorithm(i);
		res.push_back(std::make_pair(ct, nt));
		task.reset("community.dat");
	}
	return res;
}
int main(void) {
#ifdef _WINDOWS
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
//	test_graph();
//	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	//auto res = testAlgorithm();
	//int k = 1;
	//for (auto &i : res) {
	//	std::cout << k++ << ": " << i.first << " " << i.second << "\n";
	//}
	test_community(8);
	return main_ret;
}
