#include "../src/matrixmap.hpp"
#include "../src/container.hpp"
#include "../src/a-star.hpp"
#include "../src/search.hpp"
#include "catch.hpp"
#include <fstream>


TEST_CASE("MatrixMap") {
    int w = 5, h = 4;
    std::vector<bool> mtx1 = {
        0, 0, 0, 0, 0, 
        0, 1, 1, 0, 0, 
        0, 0, 1, 1, 0,
        0, 0, 0, 1, 0
    };

    auto map = BinaryMatrixMap(mtx1, w, h);

    SECTION("getAdj") {
        int deg, *adj = new int[8];
        map.getAdj(0, adj, &deg);

        REQUIRE(deg == 2);
        REQUIRE(adj[0] == 1);
        REQUIRE(adj[1] == 5);
    }

    SECTION("heurDist") {
        map.setHeuristic(Heuristic::Euclidian);
        double dist = map.heurDist(0, 19);

        REQUIRE(dist == 5.0);
    }
}


TEST_CASE("OC_STL") {
    auto oc = OC_STL();
    oc.open(0, 0, 0, 5);
    oc.open(1, 0, 1, 4.56);
    oc.open(5, 0, 1, 4.56);
    
    REQUIRE(oc.isOpened(0));
    REQUIRE(oc.isOpened(5));
    REQUIRE(oc.isOpened(1));

    oc.close(1);
    try {
        oc.close(1);
    } catch(std::invalid_argument e) {
        INFO(e.what());
    }
    REQUIRE(oc.isClosed(1));
    REQUIRE_FALSE(oc.isOpened(1));
    
    auto nodeid = oc.top();
    REQUIRE(nodeid == 5);
    REQUIRE(oc.getGValue(nodeid) == 1);

    oc.pop();
    oc.pop();
    REQUIRE(oc.empty());
}


TEST_CASE("A_star") {
    int w = 7, h = 4;
    std::vector<bool> mtx1 = {
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 1, 1,
        0, 0, 1, 1, 0, 1, 0,
        0, 0, 0, 1, 0, 1, 1
    };

    auto map = BinaryMatrixMap(mtx1, w, h);
    auto oc  = OC_STL();

    map.setHeuristic(Heuristic::Euclidian);

    std::vector<int> res;

    std::vector<int> gr_truth_0_11 = {0, 7, 15};
    std::vector<int> gr_truth_4_23 = {4, 10, 2, 1, 7, 15, 23};
    std::vector<int> gr_truth_4_20 = {};

    res = A_star(map, 0, 15, oc);
    REQUIRE(res == gr_truth_0_11);

    oc = OC_STL();
    res = A_star(map, 4, 23, oc);
    REQUIRE(res == gr_truth_4_23);

    oc = OC_STL();
    res = A_star(map, 4, 20, oc);
    REQUIRE(res == gr_truth_4_20);

    oc = OC_STL();
    res = A_star(map, 0, 0, oc);
    REQUIRE(res == std::vector<int>({0}));
    
}


TEST_CASE("A_star_city") {
    auto map = BinaryMatrixMap("../test/Shanghai_2_1024.map");

    REQUIRE(map.getSize().x == 1024);
    REQUIRE(map.getSize().y == 1024);
    

    map.exportBMP("map.bmp");
    map.setHeuristic(Heuristic::Manhattan);

    int from = 0, to = 1024 * 1024 - 1;
    REQUIRE(!map.getValue(from));
    REQUIRE(!map.getValue(to));

    auto oc = OC_STL();
    std::vector<int> p = A_star(map, to, from, oc);
    WARN("A_star_city : Found path length : " + std::to_string(p.size()));

}


TEST_CASE("Search") {
    auto s = AStarSearch("../test/Shanghai_2_1024.map");

    REQUIRE(s.pointToId({2, 2}) == 1024 * 2 + 2);
    REQUIRE(s.idToPoint(1025).x == 1);
    REQUIRE(s.idToPoint(1025).y == 1);
    REQUIRE(s.idToPoint(1024 * 1024 - 1) == point({1023, 1023}));

    s.setTargets({0, 0}, {1023, 1023});
    s.setHeuristic(Heuristic::Euclidian);
    auto path = s.run();
    REQUIRE(path.size() == 1150);

    s.exportPathBMP("path.bmp");
}