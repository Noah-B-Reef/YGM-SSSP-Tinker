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

    std::size_t l_num_edges = 0;
    // now, populate the map
    keys.for_all([&l_num_edges](auto head) {
        std::vector<std::tuple<std::size_t, float>> edge_vec;
        edges.for_all([&edge_vec, &head, &l_num_edges](auto edge) {
            if (std::get<0>(edge) == head && std::get<1>(edge) != head) {
                std::tuple<std::size_t, std::size_t> potential_edge = std::make_tuple(std::get<1>(edge), 1);
                if (std::find(edge_vec.begin(), edge_vec.end(), potential_edge) != edge_vec.end()) {
                    // do nothing
                }
                else {
                    edge_vec.push_back(potential_edge);
                    ++l_num_edges;
                }
            }
        });
        adj_list insert = {edge_vec, INF};
        map.async_insert(head, insert);
    });

    std::size_t num_edges = world.all_reduce_sum(l_num_edges);
    std::size_t num_vertices = keys.size();
    if (world.rank() == 0) {
        // this is for the YGM impl
        std::cout << "source,end,weight" << std::endl;
        // this is for the Bale impl
        //std::cout << "%%MatrixMarket matrix coordinate real general" << std::endl;
        //std::cout << num_vertices << " " << num_vertices << " " << num_edges << std::endl;
    }
    world.barrier();


    map.for_all([](auto k, auto &v) {
        for (std::tuple<std::size_t, float> edge : v.edges) {
            // this is for YGM impl -> .csv
            std::cout << k << "," << std::get<0>(edge) << "," << std::get<1>(edge) << std::endl;
            // this is for Bale impl -> .mm
            //std::cout << k << " " << std::get<0>(edge) << " " << std::get<1>(edge) << std::endl;
        }
    });
}