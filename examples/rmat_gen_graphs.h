//
// Created by mfpate on 7/11/23.
//

#ifndef CLASSIC_SSSP_RMAT_GEN_GRAPHS_H
#define CLASSIC_SSSP_RMAT_GEN_GRAPHS_H

//
// Created by mfpate on 7/11/23.
//

#include <cmath>
#include <cmath>
#include<cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <mpi.h>
#include <string>
#include <tuple>
#include <vector>
#include "rmat_edge_generator.hpp"
#include <ygm/container/map.hpp>
#include <ygm/container/bag.hpp>
#include <ygm/container/set.hpp>
#include <ygm/comm.hpp>
#include <ygm/utility.hpp>
#include<unistd.h>


void generate_rmat_graph(ygm::comm &world, ygm::container::map<std::size_t, adj_list> &map, int rmat_scale, float &max_weight) {
    static float INF = std::numeric_limits<float>::infinity();

    uint64_t total_num_edges = uint64_t(1) << (uint64_t(rmat_scale + 4)); /// Number of total edges (avg 16 per vertex)
    uint64_t local_num_edges = total_num_edges / world.size() + (world.rank() < total_num_edges % world.size()); /// Number of edges each rank generates
    bool undirected = true;
    bool scramble = true;
    rmat_edge_generator rmat(world.rank(), rmat_scale, local_num_edges, 0.57, 0.19, 0.19, 0.05, scramble, undirected);

    // Build undirected version of graph
    ygm::timer step_timer{};
    ygm::timer preprocess_timer{};

    ygm::container::map<std::size_t, std::map<std::size_t, float>> edge_map(world);

    for (int i = 0; i < pow(2, rmat_scale); ++i) {
        std::map<std::size_t, float>new_map;
        edge_map.async_insert(i, new_map);
    }

    const static int EDGE_WEIGHT_LB = 1;
    const static int EDGE_WEIGHT_UB= 100;
    const static int DUMMY_WEIGHT = -1;
    srand((unsigned) time(NULL));
    //srand(7);

    // create the graph with dummy value weights that will be updated later
    auto edge_gen_iter = rmat.begin();
    auto edge_gen_end = rmat.end();
    while (edge_gen_iter != edge_gen_end) {
        auto &edge = *edge_gen_iter;
        auto vtx1 = std::get<0>(edge);
        auto vtx2 = std::get<1>(edge);

        // Providing a seed value
        if (vtx1 != vtx2) {
            edge_map.async_visit(vtx1, [](const auto head_vtx, auto &edges, auto tail_vtx, auto weight) {
                //std::tuple<std::size_t, float> to_insert = std::make_tuple(tail_vtx, DUMMY_WEIGHT);
                //edge_set.insert(to_insert);
                edges.insert({tail_vtx, DUMMY_WEIGHT});
            }, vtx2, max_weight);
            edge_map.async_visit(vtx2, [](const auto head_vtx, auto &edges, auto tail_vtx, auto weight) {
                //std::tuple<std::size_t, float> to_insert = std::make_tuple(tail_vtx, DUMMY_WEIGHT);
                //edge_set.insert(to_insert);
                edges.insert({tail_vtx, DUMMY_WEIGHT});
            }, vtx1, max_weight);
        }
        ++edge_gen_iter;
        world.barrier();
    }
    world.barrier();


    edge_map.for_all([&edge_map, &max_weight](auto vtx, auto &vtx_edges) {
        for (auto& [key, value] : vtx_edges) {
            if (vtx < key) {
                int weight = EDGE_WEIGHT_LB + (rand() % EDGE_WEIGHT_UB);
                if (weight > max_weight) {
                    max_weight = weight;
                }
                value = weight;
                edge_map.async_visit(key, [](auto tail_vtx, auto &tail_vtx_edges, auto head_vtx, auto weight) {
                    tail_vtx_edges[head_vtx] = weight;
                }, vtx, weight);
            }
        }
    });

    world.barrier();


    // fill the passed in map
    edge_map.for_all([&map](auto vtx, auto vtx_edges) {
       std::vector<std::tuple<std::size_t, float>> adj;
       for (auto [key, value] : vtx_edges) {
           adj.push_back(std::make_tuple(key, value));
       }
       // now that the vector component is complete, create the struct and add it to the map
       adj_list to_insert = {adj, INF};
       map.async_insert(vtx, to_insert);
    });

    max_weight = world.all_reduce_max(max_weight);
}

#endif //CLASSIC_SSSP_RMAT_GEN_GRAPHS_H
