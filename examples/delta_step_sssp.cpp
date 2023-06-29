//
// Created by mfpate on 6/28/23.
//
#include <ygm/comm.hpp>
#include <ygm/container/bag.hpp>
#include "ygm/container/experimental/maptrix.hpp"
#include <tuple>
#include <cmath>
#include <limits>



std::vector<std::tuple<int, float>> find_requests(ygm::comm &world, std::vector<std::tuple<int, float, bool>> tent_pairs, int delta);
std::vector<std::tuple<int, float>> find_heavy_requests(ygm::comm &world, std::vector<std::tuple<int, float, bool>> tent_pairs, int delta);
void relax_requests(ygm::comm &world, std::vector<std::tuple<int, float>>& requests, ygm::container::map<int, std::tuple<float, bool>>& tents, float upper_bound);
void relax(std::tuple<int, float>& request, ygm::container::map<int, std::tuple<float, bool>>& tents, float upper_bound);



int main(int argc, char **argv) {
    ygm::comm world(&argc, &argv);
    const float INF = std::numeric_limits<float>::infinity();

    // create the (v, tent(v), active_flag -------------------------------------------------------------
    ygm::container::map<int, std::tuple<float, bool>> tents(world);
    if (world.rank() == 0) {
        tents.async_insert(0, std::make_tuple(INF, true));
        // b1
        tents.async_insert(1, std::make_tuple(INF, true));
        // c2
        tents.async_insert(2, std::make_tuple(INF, true));
        // d3
        tents.async_insert(3, std::make_tuple(INF, true));
        // e4
        tents.async_insert(4, std::make_tuple(INF, true));
        // f5
        tents.async_insert(5,std::make_tuple(INF, true));
        // g6
        tents.async_insert(6, std::make_tuple(INF, true));
        // h7
        tents.async_insert(7, std::make_tuple(INF, true));
        // i8
        tents.async_insert(8, std::make_tuple(INF, true));
        // j9
        tents.async_insert(9, std::make_tuple(INF, true));
        // k10
        tents.async_insert(10, std::make_tuple(INF, true));
        // l11
        tents.async_insert(11, std::make_tuple(INF, true));
        // m12
        tents.async_insert(12, std::make_tuple(INF, true));

    }
    if (world.rank() == 1) {
// n13
        tents.async_insert(13, std::make_tuple(INF, true));
        // o14
        tents.async_insert(14, std::make_tuple(INF, true));
        // p15
        tents.async_insert(15, std::make_tuple(INF, true));
        // q16
        tents.async_insert(16, std::make_tuple(INF, true));
        // r17
        tents.async_insert(17, std::make_tuple(INF, true));
        // s18
        tents.async_insert(18, std::make_tuple(INF, true));
        // t19
        tents.async_insert(19, std::make_tuple(INF, true));
        // u20
        tents.async_insert(20, std::make_tuple(INF, true));
        // v21
        tents.async_insert(21, std::make_tuple(INF, true));
        // w22
        tents.async_insert(22, std::make_tuple(INF, true));
        // x23
        tents.async_insert(23, std::make_tuple(INF, true));
        // y24
        tents.async_insert(24, std::make_tuple(INF, true));

    }
    if (world.rank() == 0) {
        // z25
        tents.async_insert(25, std::make_tuple(INF, true));
        // aa26
        tents.async_insert(26, std::make_tuple(INF, true));
        // ab27
        tents.async_insert(27, std::make_tuple(INF, true));
        // ac28
        tents.async_insert(28, std::make_tuple(INF, true));
        // ad29
        tents.async_insert(29, std::make_tuple(INF, true));
        // ae30
        tents.async_insert(30, std::make_tuple(INF, true));
        // af31
        tents.async_insert(31, std::make_tuple(INF, true));
        // ag32
        tents.async_insert(32, std::make_tuple(INF, true));
        // ah33
        tents.async_insert(33, std::make_tuple(INF, true));
        // ai34
        tents.async_insert(34, std::make_tuple(INF, true));
        // aj35
        tents.async_insert(35, std::make_tuple(INF, true));
        // ak36
        tents.async_insert(36, std::make_tuple(INF, true));
    }
    world.barrier();

    // relax the source node -------------------------------------------------------------------------
    float upper_bound = 1;
    std::tuple<int, float> source = std::make_tuple(0, 0);
    relax(source, tents, upper_bound);
    world.barrier();

    bool contains_active_nodes = true;
    int x = 0;
    while(contains_active_nodes) {
        // find the local minimum on each rank ------------------------------------------------------------
        float min = INF;
        tents.for_all([&min](int node, auto &tent_flag_pair){
            // check that the node is still active, and that the tent(v) is less than the min
            if (std::get<1>(tent_flag_pair) && std::get<0>(tent_flag_pair) < min) {
                min = std::get<0>(tent_flag_pair);
            }
        });
        world.barrier();

        // find the global minimum ---------------------------------------------------------------------
        auto global_min = world.all_reduce_min(min);
        world.barrier();

        // get the upper bound on the current bucket ------------------------------------------------------
        int delta = 3;
        upper_bound = (ceil(global_min / delta) + 1) * delta;
        world.barrier();

        std::vector<std::tuple<int, float, bool>> current_pairs;
        std::vector<std::tuple<int, float, bool>> additional_pairs;
        std::vector<std::tuple<int, float>> requests;

        requests.clear();
        current_pairs.clear();

        // get the current bucket values ----------------------------------------------------------------------------------------------
        tents.for_all([&current_pairs, &upper_bound, &world](int node, std::tuple<float, bool> &tent_flag_pair) {
            // if the node is active, and the tent is less than the upper bound
            if (std::get<1>(tent_flag_pair) && std::get<0>(tent_flag_pair) < upper_bound) {
                //std::tuple<int, float, bool> pair = std::make_tuple(node, std::get<0>(tent_flag_pair), std::get<1>(tent_flag_pair));
                current_pairs.push_back(std::make_tuple(node, std::get<0>(tent_flag_pair), std::get<1>(tent_flag_pair)));
                // assume that the node will be inactive after this iteration
                std::get<1>(tent_flag_pair) = false;
            }
        });
        world.barrier();

        // find requests ---------------------------------------------------------------------------------
        requests = find_requests(world, current_pairs, delta);
        world.barrier();

        do {
            // relax requests --------------------------------------------------------------------------------
            relax_requests(world, requests, tents, upper_bound);
            world.barrier();

            additional_pairs.clear();
            // get the current bucket values (checking for any additional values in the bucket) ----------------------------------------------------------------------------------------------
            tents.for_all([&additional_pairs, &upper_bound, &world](int node, std::tuple<float, bool> &tent_flag_pair) {
                // if the node is active, and the tent is less than the upper bound
                if (std::get<1>(tent_flag_pair) && std::get<0>(tent_flag_pair) < upper_bound) {
                    //std::tuple<int, float, bool> pair = std::make_tuple(node, std::get<0>(tent_flag_pair), std::get<1>(tent_flag_pair));
                    additional_pairs.push_back(std::make_tuple(node, std::get<0>(tent_flag_pair), std::get<1>(tent_flag_pair)));
                    // assume that the node will be inactive after this iteration
                    std::get<1>(tent_flag_pair) = false;
                }
            });
            world.barrier();

            // find requests ---------------------------------------------------------------------------------
            requests.clear();
            requests = find_requests(world, additional_pairs, delta);
            world.barrier();

            // add the new pairs to the current bucket (for heavy relaxations) --------------------------------
            for (std::tuple<int, float, bool> pair : additional_pairs) {
                current_pairs.push_back(pair);
            }
        } while (requests.size() != 0);


        // thoughts: need to recheck for the potential pairs in the current bucket (don't clear)
        // re-find requests -> check for light edges again, where
        // now do heavy
        requests = find_heavy_requests(world, current_pairs, delta);
        world.barrier();

        // relax requests --------------------------------------------------------------------------------
        relax_requests(world, requests, tents, upper_bound);
        world.barrier();


        int is_active = 0;
        // check to see if there are still active nodes
        tents.for_all([&is_active](int node, auto &value) {
           // if it is active, the is_active is flagged - more iterations must occur
           if (int(std::get<1>(value)) > is_active) {
               is_active = int(std::get<1>(value));
           }
        });

        contains_active_nodes = bool(world.all_reduce_max(is_active));
        // now, all relaxations for a given bucket are done, so repeat the process with a new bucket/upper bound
        ++x;
    }

    // print out all the v tent(v) pairs
    tents.for_all([](int node, auto &value) {
        std::cout << "tent(" << node << ") = " << std::get<0>(value) << std::endl;
    });

}

void relax(std::tuple<int, float>& request, ygm::container::map<int, std::tuple<float, bool>>& tents, float upper_bound) {
    bool repeat = false;
    auto update_tent = [](auto node, auto &value, std::tuple<int, float> request, float upper_bound) {
        // if the proposed tent from request is less than the current tent
        if (std::get<1>(request) < std::get<0>(value)) {
            std::get<0>(value) = std::get<1>(request);
            // if the node re-enters the same bucket, the new tent(v) < upper bound
            if (std::get<0>(value) < upper_bound) {
                std::get<1>(value) = true;
            }
        }
    };
    tents.async_visit(std::get<0>(request), update_tent, request, upper_bound);
}

void relax_requests(ygm::comm &world, std::vector<std::tuple<int, float>>& requests, ygm::container::map<int, std::tuple<float, bool>>& tents, float upper_bound) {
    for (std::tuple<int, float> request : requests) {
        relax(request, tents, upper_bound);
    }
    world.barrier();
}

std::vector<std::tuple<int, float>> find_requests(ygm::comm &world, std::vector<std::tuple<int, float, bool>> tent_pairs, int delta) {
// here is the matrix
    ygm::container::map<int, std::vector<std::tuple<int, float>>> mat(world);
    std::vector<std::tuple<int, float>> adj_list;

    if (world.rank() == 0) {
        adj_list.clear();
        adj_list.insert(adj_list.end(), {std::make_tuple(1, 3), std::make_tuple(2, 3), std::make_tuple(3, 5), std::make_tuple(4, 3)});
        mat.async_insert(0, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(0, 3), std::make_tuple(5, 3), std::make_tuple(6, 2), std::make_tuple(7, 3)});
        mat.async_insert(1, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(0, 3), std::make_tuple(8, 2), std::make_tuple(9, 5), std::make_tuple(10, 4)});
        mat.async_insert(2, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(0, 5), std::make_tuple(5, 1), std::make_tuple(11, 2), std::make_tuple(12, 1), std::make_tuple(13, 1)});
        mat.async_insert(3, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(0, 3), std::make_tuple(14, 2), std::make_tuple(15, 2), std::make_tuple(16, 3), std::make_tuple(17, 3)});
        mat.async_insert(4, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(1, 3), std::make_tuple(3, 1)});
        mat.async_insert(5, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(1, 3), std::make_tuple(34, 5), std::make_tuple(19, 1), std::make_tuple(32, 2), std::make_tuple(33, 1)});
        mat.async_insert(6, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(1, 3), std::make_tuple(27, 1), std::make_tuple(28, 3)});
        mat.async_insert(7, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(2, 2), std::make_tuple(35, 4), std::make_tuple(36, 4)});
        mat.async_insert(8, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(2, 5)});
        mat.async_insert(9, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(2, 4), std::make_tuple(21, 3), std::make_tuple(18, 3), std::make_tuple(23, 2), std::make_tuple(20, 1)});
        mat.async_insert(10, adj_list);
        adj_list.clear();
    }
    if (world.rank() == 1) {
        adj_list.clear();
        adj_list.insert(adj_list.end(), {std::make_tuple(3, 2), std::make_tuple(31, 1)});
        mat.async_insert(11, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(3, 1)});
        mat.async_insert(12, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(3, 1), std::make_tuple(31, 1), std::make_tuple(29, 3), std::make_tuple(30, 2)});
        mat.async_insert(13, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(4, 2), std::make_tuple(26, 1)});
        mat.async_insert(14, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(4, 2)});
        mat.async_insert(15, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(4, 3), std::make_tuple(20, 6), std::make_tuple(25, 4)});
        mat.async_insert(16, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(4, 3)});
        mat.async_insert(17, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(10, 3)});
        mat.async_insert(18, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(6, 1)});
        mat.async_insert(19, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(10, 1), std::make_tuple(16, 6), std::make_tuple(22, 3), std::make_tuple(24, 2)});
        mat.async_insert(20, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(10, 3)});
        mat.async_insert(21, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(20, 3)});
        mat.async_insert(22, adj_list);
    }
    if (world.rank() == 1) {
        adj_list.clear();
        adj_list.insert(adj_list.end(), {std::make_tuple(10, 2)});
        mat.async_insert(23, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(20, 2)});
        mat.async_insert(24, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(16, 4)});
        mat.async_insert(25, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(14, 1)});
        mat.async_insert(26, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(7, 1)});
        mat.async_insert(27, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(7, 3), std::make_tuple(33, 1)});
        mat.async_insert(28, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(13, 3)});
        mat.async_insert(29, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(13, 2)});
        mat.async_insert(30, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(13, 1), std::make_tuple(11, 1)});
        mat.async_insert(31, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(6, 2)});
        mat.async_insert(32, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(6, 1), std::make_tuple(28, 1)});
        mat.async_insert(33, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(6, 5)});
        mat.async_insert(34, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(8, 4)});
        mat.async_insert(35, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(8, 4)});
        mat.async_insert(36, adj_list);
    }

    world.barrier();

    // assume that you are doing heavy requests until a light request is found
    bool is_light = false;
    std::vector<std::tuple<int, float>> requests;

    mat.for_all([&tent_pairs, &requests, &delta, &is_light](auto &row, auto &adj) {
        for (std::tuple<int, float, bool> tent_pair : tent_pairs) {
            // check that the current node is still active
            if (row == std::get<0>(tent_pair) && std::get<2>(tent_pair)) {
                for (int i = 0; i < adj.size(); ++i) {
                    // if tracking heavy, but come across a light, clear the current requests, and start tracking light
                    // and the current
                    if (!is_light && std::get<1>(adj[i]) <= delta) {
                        requests.clear();
                        is_light = true;
                        requests.push_back(std::make_tuple(std::get<0>(adj[i]), std::get<1>(tent_pair) + std::get<1>(adj[i])));
                    }
                    // otherwise, if the flag matches the node type, add it to the requests vec
                    else if (is_light && std::get<1>(adj[i]) <= delta) {
                        requests.push_back(std::make_tuple(std::get<0>(adj[i]), std::get<1>(tent_pair) + std::get<1>(adj[i])));
                    }
                    // nothing happens for light flags with heavy nodes
                }
            }
        }
    });
    world.barrier();

    //return requests;
    return requests;
}

std::vector<std::tuple<int, float>> find_heavy_requests(ygm::comm &world, std::vector<std::tuple<int, float, bool>> tent_pairs, int delta) {
// here is the matrix
    ygm::container::map<int, std::vector<std::tuple<int, float>>> mat(world);
    std::vector<std::tuple<int, float>> adj_list;

    if (world.rank() == 0) {
        adj_list.clear();
        adj_list.insert(adj_list.end(), {std::make_tuple(1, 3), std::make_tuple(2, 3), std::make_tuple(3, 5), std::make_tuple(4, 3)});
        mat.async_insert(0, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(0, 3), std::make_tuple(5, 3), std::make_tuple(6, 2), std::make_tuple(7, 3)});
        mat.async_insert(1, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(0, 3), std::make_tuple(8, 2), std::make_tuple(9, 5), std::make_tuple(10, 4)});
        mat.async_insert(2, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(0, 5), std::make_tuple(5, 1), std::make_tuple(11, 2), std::make_tuple(12, 1), std::make_tuple(13, 1)});
        mat.async_insert(3, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(0, 3), std::make_tuple(14, 2), std::make_tuple(15, 2), std::make_tuple(16, 3), std::make_tuple(17, 3)});
        mat.async_insert(4, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(1, 3), std::make_tuple(3, 1)});
        mat.async_insert(5, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(1, 3), std::make_tuple(34, 5), std::make_tuple(19, 1), std::make_tuple(32, 2), std::make_tuple(33, 1)});
        mat.async_insert(6, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(1, 3), std::make_tuple(27, 1), std::make_tuple(28, 3)});
        mat.async_insert(7, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(2, 2), std::make_tuple(35, 4), std::make_tuple(36, 4)});
        mat.async_insert(8, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(2, 5)});
        mat.async_insert(9, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(2, 4), std::make_tuple(21, 3), std::make_tuple(18, 3), std::make_tuple(23, 2), std::make_tuple(20, 1)});
        mat.async_insert(10, adj_list);
        adj_list.clear();
    }
    if (world.rank() == 1) {
        adj_list.clear();
        adj_list.insert(adj_list.end(), {std::make_tuple(3, 2), std::make_tuple(31, 1)});
        mat.async_insert(11, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(3, 1)});
        mat.async_insert(12, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(3, 1), std::make_tuple(31, 1), std::make_tuple(29, 3), std::make_tuple(30, 2)});
        mat.async_insert(13, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(4, 2), std::make_tuple(26, 1)});
        mat.async_insert(14, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(4, 2)});
        mat.async_insert(15, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(4, 3), std::make_tuple(20, 6), std::make_tuple(25, 4)});
        mat.async_insert(16, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(4, 3)});
        mat.async_insert(17, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(10, 3)});
        mat.async_insert(18, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(6, 1)});
        mat.async_insert(19, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(10, 1), std::make_tuple(16, 6), std::make_tuple(22, 3), std::make_tuple(24, 2)});
        mat.async_insert(20, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(10, 3)});
        mat.async_insert(21, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(20, 3)});
        mat.async_insert(22, adj_list);
    }
    if (world.rank() == 1) {
        adj_list.clear();
        adj_list.insert(adj_list.end(), {std::make_tuple(10, 2)});
        mat.async_insert(23, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(20, 2)});
        mat.async_insert(24, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(16, 4)});
        mat.async_insert(25, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(14, 1)});
        mat.async_insert(26, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(7, 1)});
        mat.async_insert(27, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(7, 3), std::make_tuple(33, 1)});
        mat.async_insert(28, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(13, 3)});
        mat.async_insert(29, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(13, 2)});
        mat.async_insert(30, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(13, 1), std::make_tuple(11, 1)});
        mat.async_insert(31, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(6, 2)});
        mat.async_insert(32, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(6, 1), std::make_tuple(28, 1)});
        mat.async_insert(33, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(6, 5)});
        mat.async_insert(34, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(8, 4)});
        mat.async_insert(35, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(8, 4)});
        mat.async_insert(36, adj_list);
    }

    world.barrier();

    // assume that you are doing heavy requests until a light request is found
    bool is_light = false;
    std::vector<std::tuple<int, float>> requests;

    mat.for_all([&tent_pairs, &requests, &delta, &is_light](auto &row, auto &adj) {
        for (std::tuple<int, float, bool> tent_pair : tent_pairs) {
            // check that the current node is still active
            if (row == std::get<0>(tent_pair) && std::get<2>(tent_pair)) {
                for (int i = 0; i < adj.size(); ++i) {
                    if (!is_light && std::get<1>(adj[i]) > delta) {
                        requests.push_back(std::make_tuple(std::get<0>(adj[i]), std::get<1>(tent_pair) + std::get<1>(adj[i])));
                    }
                    // nothing happens for light flags with heavy nodes
                }
            }
        }
    });
    world.barrier();

    //return requests;
    return requests;
}