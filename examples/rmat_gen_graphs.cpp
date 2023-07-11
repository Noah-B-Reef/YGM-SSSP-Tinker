//
// Created by mfpate on 7/11/23.
//
#include "adjacency.h"
#include <cmath>
#include <iostream>
#include <limits>
#include "rmat_edge_generator.hpp"
#include <ygm/container/map.hpp>
#include <ygm/utility.hpp>

int main(int argc, char **argv) {
    ygm::comm world(&argc, &argv);

    float INF = std::numeric_limits<float>::infinity();
    int rmat_scale = atoi(argv[1]);

    uint64_t total_num_edges = 8;//uint64_t(1) << (uint64_t(rmat_scale + 4)); /// Number of total edges (avg 16 per vertex)
    uint64_t local_num_edges = total_num_edges / world.size() + (world.rank() < total_num_edges % world.size()); /// Number of edges each rank generates
    bool undirected = true;
    bool scramble = true;
    rmat_edge_generator rmat(world.rank(), rmat_scale, local_num_edges, 0.57, 0.19, 0.19, 0.05, scramble, undirected);

    // Build undirected version of graph
    world.cout0("Generating undirected RMAT graph scale ", rmat_scale);
    ygm::timer step_timer{};
    ygm::timer preprocess_timer{};

    // Graph with ints for IDs, and bools as dummy vertex and edge metadata
    ygm::container::map<std::size_t, adj_list> map(world);


    auto edge_gen_iter = rmat.begin();
    auto edge_gen_end = rmat.end();
    while (edge_gen_iter != edge_gen_end) {
        auto &edge = *edge_gen_iter;

        auto vtx1 = std::get<0>(edge);
        auto vtx2 = std::get<1>(edge);

        // check to see if that vtx1 is already a key in the map
        if (map.count(vtx1) > 0) {
            // append this edge to the existing list of edges for that node
            map.async_visit(vtx1, [](auto &node, auto &node_info, const auto &tail) {
                // if the edge is not already in the list, push_back
                bool exists = false;
                for (std::tuple<std::size_t, float> edge : node_info.edges) {
                    if (std::get<0>(edge) == tail) {
                        exists = true;
                    }
                }
                // if the edge does not exist, insert it
                if (!exists) {
                    std::tuple<std::size_t, float> to_insert = std::make_tuple(tail, 1);
                    node_info.edges.push_back(to_insert);
                }
            }, vtx2);
        }
        else {
            std::tuple<std::size_t, float> to_insert = std::make_tuple(vtx2, 1);
            std::vector<std::tuple<std::size_t, float>> edges;
            edges.push_back(to_insert);
            adj_list new_adj_list = {edges, INF};
            map.async_insert(vtx1, new_adj_list);
        }
        //adj_list vtx_info = {vtx2, 1};
        //map.async_insert(vtx1, vtx_info);
        //std::cout << "vtx1: " << vtx1 << ", vtx2: " << vtx2 << std::endl;
        ++edge_gen_iter;
    }

    map.for_all([](auto k, auto v) {
        for (std::tuple<std::size_t, float> edge : v.edges) {
            std::cout << "{" << k << ", " << std::get<0>(edge) << "} -> tent = " << v.tent << std::endl;
        }
    });

    world.cout0("RMAT generation time: ", step_timer.elapsed(), " seconds");
}