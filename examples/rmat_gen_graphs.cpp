//
// Created by mfpate on 7/11/23.
//
#include "adjacency.h"
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
    //std::string prefix(argv[2]);

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

    int weight = EDGE_WEIGHT_LB + (rand() % EDGE_WEIGHT_UB);
    auto max_weight = world.all_reduce_max(weight);

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

    edge_map.for_all([](auto head_vtx, auto edge_set) {
        for (auto edge : edge_set) {
            std::cout << head_vtx << "," << std::get<0>(edge) << std::endl;
        }
    });


    edge_map.for_all([&edge_map](auto vtx, auto &vtx_edges) {
        for (auto& [key, value] : vtx_edges) {
            if (vtx < key) {
                int weight = EDGE_WEIGHT_LB + (rand() % EDGE_WEIGHT_UB);
                value = weight;
                edge_map.async_visit(key, [](auto tail_vtx, auto &tail_vtx_edges, auto head_vtx, auto weight) {
                    tail_vtx_edges[head_vtx] = weight;
                }, vtx, weight);
            }
        }
    });

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
    /*ofstream outfile;
    int local_id = world.rank();
    std::string outfile_name = prefix + "." + std::to_string(local_id) + ".out";
    outfile.open(outfile_name);
    world.barrier();

    edge_map.for_all([&outfile](auto head_vtx, auto edge_set) {
        for (auto edge : edge_set) {
            outfile << head_vtx << "," << std::get<0>(edge) << "," << std::get<1>(edge) << std::endl;
        }
    });

    /*for (int k = 0; k < offset; k++) {
        outfile << std::to_string(local_id + k) << endl;
    }
    world.barrier();

    outfile.close();
// -----------------------------------------------------------------------------------------------------
    ifstream fin;
    fin.open(outfile_name);

    int max = 0;
    int max_weight = 0;
    ygm::container::map<std::size_t, adj_list> mat (world);

    ygm::container::map<std::size_t, std::map<std::size_t, float>> m(world);

    for (int i = 0; i < pow(2, rmat_scale); ++i) {
        std::map<std::size_t, float>new_map;
        m.async_insert(i, new_map);
    }

    std::vector <std::string> row;
    std::vector <std::tuple<std::size_t, float>> adj;
    std::string line, word, temp;

    // keep track of current node's adjacency list
    std::size_t curr_node = 0;

    // skip first line
    getline(fin, line);

    while (getline(fin, line)) {
        row.clear();

        // breaking words
        std::stringstream s(line);

        // read column data
        while(std::getline(s, word,char(','))) {
            row.push_back(word);
        }

        /*m.async_visit(std::stoi(row[0]), [](auto vtx, auto &vtx_map, auto tail, auto weight) {
            vtx_map.insert({tail, weight});
        }, std::stoi(row[1]), std::stoi(row[2]));

        std::cout << row[0] << " " << row[1] << " " << row[2] << std::endl;
    }

    world.barrier();
    fin.close();

    m.for_all([&mat](auto vtx, auto vtx_edges) {
        std::vector <std::tuple<std::size_t, float>> adj;
        for (auto e : vtx_edges) {
            adj.push_back(std::make_tuple(std::get<0>(e), std::get<1>(e)));
        }
        adj_list to_insert = {adj, INF};
        mat.async_insert(vtx, to_insert);
    });

    mat.for_all([](auto node, auto adj) {
        for (std::tuple<size_t, float> e : adj.edges) {
            std::cout << node << " " << std::get<0>(e) << " " << std::get<1>(e) << std::endl;
        }
    });*/

}