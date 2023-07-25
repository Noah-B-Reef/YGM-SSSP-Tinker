//
// Created by mfpate on 7/3/23.
//

# define _GLIBCXX_USE_CXX11_ABI 0
#include <cmath>
#include <tuple>
#include <vector>
#include <ygm/for_all_adapter.hpp>
#include <ygm/comm.hpp>
#include <ygm/container/set.hpp>
#include <ygm/container/map.hpp>
#include "adjacency.h"
#include "rmat_gen_graphs.h"
#include <chrono>

int main(int argc, char* argv[]) {
    ygm::comm world(&argc, &argv);

    // the array of sets of vertices
    std::vector<ygm::container::set<std::size_t>> buckets;
    static ygm::container::map<std::size_t, adj_list> map(world);

    // THIS WILL NEED TO BE CHANGED!!
    std::size_t num_buckets = 0;
    static float delta = 3;
    float max_weight = 0; // the maximum shortest path weight -> 21 for testing
    float max_degree;
    
    if (argc > 1) {
        //num_buckets = std::atoi(argv[1]); // = ceil(max_cost / delta) + 2; -> 9 for testing
        //static float delta = std::atoi(argv[2]); // -> 3 for testing
       // std::string path = argv[1];
       int rmat_scale = std::atoi(argv[1]);
        // here is the lookup map for vertices and their best tent values/adj list (as a struct)
        //getGraph(world, map, max_weight, path);

        generate_rmat_graph(world, map, rmat_scale, max_weight);

        int degree = 0;


        map.for_all([&degree](auto k, auto v) {
            if (v.edges.size() > degree)
            {
                degree = v.edges.size();
            }
        });

        float max_degree = world.all_reduce_max(degree);
        static float delta = 1.0/max_degree;
        num_buckets = (size_t)ceil(max_weight/delta) + 1;
    }
    else {

        std::string path = "";
        int degree = 0;

        // here is the lookup map for vertices and their best tent values/adj list (as a struct)
        //getGraph(world, map, max_weight, path);
	    generate_rmat_graph(world, map, 8, max_weight);

        map.for_all([&degree](auto k, auto v)
        {
            if (v.edges.size() > degree)
            {
                degree = v.edges.size();
            }
        });

        float max_degree = world.all_reduce_max(degree);

        static float delta = 1.0/max_degree;
        
        num_buckets = (size_t)ceil(max_weight/delta) + 1;
    }

    map.for_all([](auto &vertex, adj_list &vertex_info) {
        for (auto e : vertex_info.edges) {
            std::cout << "" << vertex << ", " << std::get<0>(e) << " > " << std::get<1>(e) << std::endl;
        }
    });

    // add the sets to the vector -------------------------------------------------------------------------------------
    for (int i = 0; i < num_buckets; ++i) {
        buckets.emplace_back(world);
    }

    // start timing
    auto beg = std::chrono::high_resolution_clock::now();

    int idx = 0;
    // complete a source relaxation --------------------------------------------------------------------------------------
    // relax the source
    map.async_visit(0, [](auto &source, auto &source_info) {
        source_info.tent = 0;
    });
    // insert the source into the first bucket
    buckets[0].async_insert(0);
    
    // duplicate the current bucket -----------------------------------------------------------
    ygm::container::set<std::size_t> bucket_copy(world);

    static auto fill_bucket_copy_lambda = [&bucket_copy](const auto &vertex) {
        // if the vertex is not already in the copy bucket, add it
        bucket_copy.async_insert(vertex);
    };

    buckets[idx].for_all([](const auto &vertex) {
       fill_bucket_copy_lambda(vertex);
    });

    // process the current bucket ---------------------------------------------------------------
    static auto add_vertex_to_bucket_lambda = [&buckets](auto &vertex, auto &idx) {
        buckets[idx].async_insert(vertex);
    };

    // For a given node (this is the tail of an edge from a node in the current bucket),
    // update the tent value if the potential is better
    static auto relax_requests_lambda = [](auto &vertex, auto &potential_tent) {
        map.async_visit(vertex, [](auto &node, adj_list &node_info, auto &potential_tent) {
            if (potential_tent < node_info.tent) {
                // update the tentative
                node_info.tent = potential_tent;
                int idx = floor(node_info.tent / delta);
                add_vertex_to_bucket_lambda(node, idx);
            }
        }, potential_tent);
    };

    // Remove a given node from the current bucket
    static auto remove_from_bucket_lambda = [&buckets, &idx](const auto &vertex) {
        buckets[idx].async_erase(vertex);
    };

    // for all vertices in the current bucket:
    // first, remove them from the bucket
    // then, go to that row in the map, walk through its adjacency list and relax all requests
    while (idx < num_buckets) {
        // check to see if there is even anything in the current bucket
        // this wrapper prevents the race condition on re-insertions to the same bucket
        ygm::consume_all_iterative_adapter curr_bucket_wrapper(buckets[idx]);
        curr_bucket_wrapper.consume_all([](auto vertex) {
            // add all vertices in the current bucket to the copy
            fill_bucket_copy_lambda(vertex);

            // remove the current vertex from the current bucket
            //remove_from_bucket_lambda(vertex);

            // go to that row in the map and relax requests
            map.async_visit(vertex, [](const auto &head, adj_list &head_info) {
                for (auto edge : head_info.edges) {
                    if (std::get<1>(edge) <= delta) {
                        float potential_tent = head_info.tent + std::get<1>(edge);
                        relax_requests_lambda(std::get<0>(edge), potential_tent);
                    }
                }
            });
        });

        // do the heavy relaxations (only one round) ---------------------------------------------------------------------
        bucket_copy.consume_all([](auto vertex) {
            // go to that row in the map and relax requests
            map.async_visit(vertex, [](const auto &head, adj_list &head_info) {
                for (auto edge : head_info.edges) {
                    if (std::get<1>(edge) > delta) {
                        float potential_tent = head_info.tent + std::get<1>(edge);
                        relax_requests_lambda(std::get<0>(edge), potential_tent);
                    }
                }
            });
        });
        world.barrier();
        // done with this bucket
        ++idx;
        bucket_copy.clear();
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
    
    /*
    // print out the final distances from the source for each node
    map.for_all([](auto &vertex, adj_list &vertex_info) {
        std::cout << "" << vertex << ", " << vertex_info.tent << std::endl;
    });
    */
}
