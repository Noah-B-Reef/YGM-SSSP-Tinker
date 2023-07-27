//
// Created by mfpate on 7/13/23.
//

# define _GLIBCXX_USE_CXX11_ABI 0
#include <cmath>
#include <tuple>
#include <vector>
#include <ygm/comm.hpp>
#include <ygm/container/set.hpp>
#include <ygm/container/map.hpp>
#include "adjacency.h"
#include "rmat_gen_graphs.h"
#include <chrono>


int main(int argc, char* argv[]) {
    ygm::comm world(&argc, &argv);

    static ygm::container::map<std::size_t, std::tuple<std::size_t, bool>> successors(world);
    static ygm::container::map<std::size_t, adj_list> map(world);

    std::string path = "/home/molliep/ygm/examples/data/data1.csv";
    float max_weight;

    int rmat_scale = std::atoi(argv[1]);
    // fill the graph
    /*if (argc > 1) {
        //path = argv[1];
    }*/
    //getGraph(world, map, max_weight, path);
    generate_rmat_graph(world, map, rmat_scale, max_weight);


    // fill the successors -> default is -1
    map.for_all([](auto vertex, auto &vertex_info) {
       successors.async_insert(vertex, std::make_tuple(-1, false));
    });


    map.async_visit(0, [](auto vertex, auto &vertex_info) {
        vertex_info.tent = 0;
    });

    successors.async_visit(0, [](auto vertex, auto &neighbor) {
       std::get<0>(neighbor) = 0;
       std::get<1>(neighbor) = true;
    });

    int N = map.size();
    bool unprocessed_updates = false;
    bool global_unprocessed_updates = true;


    // start timing
    auto beg = std::chrono::high_resolution_clock::now();

    //for (int i = 0; i < N - 1; ++i) { -->> this is for the standard version (vertices - 1 iterations)
    while (global_unprocessed_updates) {
        successors.for_all([&unprocessed_updates](auto vertex, auto &was_visited) {
           if (std::get<1>(was_visited)) {
               unprocessed_updates = true;
               // set it to unvisited
               std::get<1>(was_visited) = false;
               map.async_visit(vertex, [](auto vertex, auto &vertex_info) {
                   for (std::tuple<std::size_t, float> edge : vertex_info.edges) {
                       float potential_tent = std::get<1>(edge) + vertex_info.tent;
                       map.async_visit(std::get<0>(edge), [](auto tail, auto &tail_info, auto potential_tent, auto head) {
                            if (potential_tent < tail_info.tent) {
                                tail_info.tent = potential_tent;
                                successors.async_visit(tail, [] (auto vertex, auto &successor, auto head) {
                                    get<0>(successor) = head;
                                    get<1>(successor) = true;
                                }, head);
                            }
                       }, potential_tent, vertex);
                   }
               });
           }
        });

        // check to see if there were changes in the current iteration
        global_unprocessed_updates = bool(world.all_reduce_max(int(unprocessed_updates)));
        unprocessed_updates = false;
    }

    // end timing
    auto end = std::chrono::high_resolution_clock::now();

    // compute total elapsed time
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - beg);

    int time = duration.count();

    auto global_time = world.all_reduce_max(time);


    int num_edges = 0;

    map.for_all([&num_edges](auto key, auto &vertex){
        num_edges += vertex.edges.size();
    });

    int global_num_edges = world.all_reduce_sum(num_edges);

    if (world.rank() == 0)
    {
        std::cout << global_time/1000.0 << std::endl;
        std::cout << global_num_edges << std::endl;
    }

    /*// print the results
    map.for_all([](auto vertex, auto &vertex_info) {
        std::cout << "tent(" << vertex << ") = " << vertex_info.tent << std::endl;
    });*/
}
