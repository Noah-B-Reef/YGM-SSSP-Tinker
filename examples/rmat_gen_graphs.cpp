//
// Created by mfpate on 7/11/23.
//
#include "adjacency.h"
#include <cmath>
#include<cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <tuple>
#include <vector>
#include "rmat_edge_generator.hpp"
#include <ygm/container/map.hpp>
#include <ygm/container/bag.hpp>
#include <ygm/container/set.hpp>
#include <ygm/comm.hpp>
#include <ygm/utility.hpp>
#include<unistd.h>               // for linux

/*
 * NOTES ON HOW TO RUN:
 * this program can generate output files for both YGM and Bale implementations, depending on
 * which lines are uncommented.
 *
 * For the YGM implementation files, uncomment the lines that are commented "this is for the YGM impl"
 * and for Bale, uncomment the lines "this is for the bale impl"
 *
 * Compile, and then on the command line, salloc and run:
 *      For YGM:
 *          srun -o {PATH TO DESIRED OUTPUT FILE}/rmat_gen_graph.csv examples/rmat_gen_graphs {rmat_scale}
 *      For Bale:
 *          srun -o {PATH TO DESIRED OUTPUT FILE}/rmat_gen_graph.mm examples/rmat_gen_graphs {rmat_scale}
 *
 * Now, go to the specified location, and your file will be there
 *
 */

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
    ygm::timer step_timer{};
    ygm::timer preprocess_timer{};

    ygm::container::map<std::size_t, std::set<std::tuple<std::size_t, float>>> edge_map(world);
    for (int i = 0; i < pow(2, rmat_scale); ++i) {
        std::set<std::tuple<std::size_t, float>>new_set({});
        edge_map.async_insert(i, new_set);
    }

    const static int EDGE_WEIGHT_LB = 1;
    const static int EDGE_WEIGHT_UB= 100;
    const static int DUMMY_WEIGHT = -1;
    srand((unsigned) time(NULL));

    // create the graph with dummy value weights that will be updated later
    auto edge_gen_iter = rmat.begin();
    auto edge_gen_end = rmat.end();
    while (edge_gen_iter != edge_gen_end) {
        auto &edge = *edge_gen_iter;
        auto vtx1 = std::get<0>(edge);
        auto vtx2 = std::get<1>(edge);


        // Providing a seed value
        if (vtx1 != vtx2) {
            edge_map.async_visit(vtx1, [](const auto head_vtx, auto &edge_set, auto tail_vtx) {
                std::tuple<std::size_t, float> to_insert = std::make_tuple(tail_vtx, DUMMY_WEIGHT);
                edge_set.insert(to_insert);
            }, vtx2);
            edge_map.async_visit(vtx2, [](const auto head_vtx, auto &edge_set, auto tail_vtx) {
                std::tuple<std::size_t, float> to_insert = std::make_tuple(tail_vtx, DUMMY_WEIGHT);
                edge_set.insert(to_insert);
            }, vtx1);

        }
        world.barrier();
        ++edge_gen_iter;
    }
    world.barrier();

    // add the weights to all existing edges
    for (int vtx1 = 0; vtx1 < pow(2, rmat_scale); ++vtx1) {
        for (int vtx2 = 0; vtx2 < pow(2, rmat_scale); ++vtx2) {
            if (vtx1 != vtx2) {
                int random = EDGE_WEIGHT_LB + (rand() % EDGE_WEIGHT_UB);
                edge_map.async_visit(vtx1, [](const auto head_vtx, auto &edge_set, auto tail_vtx, auto weight) {
                    std::set<std::tuple<std::size_t, float>>::iterator it;
                    for (it = edge_set.begin(); it != edge_set.end(); it ++) {
                        if (std::get<0>(*it) == tail_vtx && std::get<1>(*it) == DUMMY_WEIGHT) {
                            edge_set.erase(*it);
                            edge_set.insert(std::tuple(tail_vtx, weight));
                        }
                    }

                }, vtx2, random);
                edge_map.async_visit(vtx2, [](const auto head_vtx, auto &edge_set, auto tail_vtx, auto weight) {
                    std::set<std::tuple<std::size_t, float>>::iterator it;
                    for (it = edge_set.begin(); it != edge_set.end(); it ++) {
                        if (std::get<0>(*it) == tail_vtx && std::get<1>(*it) == DUMMY_WEIGHT) {
                            edge_set.erase(*it);
                            edge_set.insert(std::tuple(tail_vtx, weight));
                        }
                    }
                }, vtx1, random);
            }
        }
    }
    world.barrier();

    // print out the edges
    if (world.rank() == 0) {
        // for YGM impl
        std::cout << "source,end,weight" << std::endl;
    }
    world.barrier();

    edge_map.for_all([](auto head_vtx, auto edge_set) {
        for (auto edge : edge_set) {
            std::cout << head_vtx << "," << std::get<0>(edge) << "," << std::get<1>(edge) << std::endl;
        }
    });

}