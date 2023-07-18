//
// Created by mfpate on 7/11/23.
//
#include "adjacency.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <tuple>
#include "rmat_edge_generator.hpp"
#include <ygm/container/map.hpp>
#include <ygm/container/bag.hpp>
#include <ygm/container/set.hpp>
#include <ygm/comm.hpp>
#include <ygm/utility.hpp>


int main(int argc, char **argv) {
    ygm::comm world(&argc, &argv);

    static float INF = std::numeric_limits<float>::infinity();
    int rmat_scale = atoi(argv[1]);

    uint64_t total_num_edges = uint64_t(1) << (uint64_t(rmat_scale + 4)); /// Number of total edges (avg 16 per vertex)
    uint64_t local_num_edges = total_num_edges / world.size() + (world.rank() < total_num_edges % world.size()); /// Number of edges each rank generates
    bool undirected = true;
    bool scramble = true;
    rmat_edge_generator rmat(world.rank(), rmat_scale, local_num_edges, 0.57, 0.19, 0.19, 0.05, scramble, undirected);

    // Build undirected version of graph
    //world.cout0("Generating undirected RMAT graph scale ", rmat_scale);
    ygm::timer step_timer{};
    ygm::timer preprocess_timer{};

    // Graph with ints for IDs, and bools as dummy vertex and edge metadata
    static ygm::container::map<std::size_t, adj_list> map(world);


    ygm::container::set<std::size_t> keys(world);
    static ygm::container::bag<std::tuple<std::size_t, std::size_t>> edges(world);


    auto edge_gen_iter = rmat.begin();
    auto edge_gen_end = rmat.end();
    while (edge_gen_iter != edge_gen_end) {
        auto &edge = *edge_gen_iter;
        auto vtx1 = std::get<0>(edge);
        auto vtx2 = std::get<1>(edge);

        keys.async_insert(vtx1);
        std::tuple<std::size_t, std::size_t> test = std::make_tuple(vtx1, vtx2);
        edges.async_insert(test);

        ++edge_gen_iter;
    }

    // now, populate the map
    keys.for_all([](auto head) {
        std::vector<std::tuple<std::size_t, float>> edge_vec;
        edges.for_all([&edge_vec, &head](auto edge) {
            if (std::get<0>(edge) == head && std::get<1>(edge) != head) {
                std::tuple<std::size_t, std::size_t> potential_edge = std::make_tuple(std::get<1>(edge), 1);
                if (std::find(edge_vec.begin(), edge_vec.end(), potential_edge) != edge_vec.end()) {
                    // do nothing
                }
                else {
                    edge_vec.push_back(potential_edge);
                }
            }
        });
        adj_list insert = {edge_vec, INF};
        map.async_insert(head, insert);
    });


    std::cout << "%%MatrixMarket matrix coordinate real general" << std::endl;
    map.for_all([](auto k, auto &v) {
        for (std::tuple<std::size_t, float> edge : v.edges) {
            // this is for YGM impl -> .csv
            std::cout << k << "," << std::get<0>(edge) << "," << std::get<1>(edge) << std::endl;
            // this is for Bale impl -> .mm
            //std::cout << k << " " << std::get<0>(edge) << " " << std::get<1>(edge) << std::endl;
        }
    });

    //world.cout0("RMAT generation time: ", step_timer.elapsed(), " seconds");

}
