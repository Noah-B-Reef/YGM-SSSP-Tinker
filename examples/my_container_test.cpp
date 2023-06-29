//
// Created by mfpate on 6/20/23.
//


#include <ygm/comm.hpp>
#include <ygm/container/bag.hpp>
#include "ygm/container/experimental/maptrix.hpp"
#include <tuple>
#include <cmath>
#include <limits>

std::vector<std::tuple<int, float>> find_requests(ygm::comm &world, std::vector<std::tuple<int, float>> tent_pairs, bool is_light, int delta);
void relax_requests(ygm::comm &world, std::vector<std::tuple<int, float>>& requests, ygm::container::map<int, float>& tents);
void relax(std::tuple<int, float>& request, ygm::container::map<int, float>& tents);

int main(int argc, char **argv) {
    ygm::comm world(&argc, &argv);

    const float inf = std::numeric_limits<float>::infinity();

    // create the lookup table
    ygm::container::map<int, float> tents(world);

    if (world.rank() == 0) {
        tents.async_insert(0, inf);
        // b1
        tents.async_insert(1, inf);
        // c2
        tents.async_insert(2, inf);
        // d3
        tents.async_insert(3, inf);
        // e4
        tents.async_insert(4, inf);
        // f5
        tents.async_insert(5,inf);
        // g6
        tents.async_insert(6, inf);
        // h7
        tents.async_insert(7, inf);
        // i8
        tents.async_insert(8, inf);
        // j9
        tents.async_insert(9, inf);
        // k10
        tents.async_insert(10, inf);
        // l11
        tents.async_insert(11, inf);
        // m12
        tents.async_insert(12, inf);

    }
    if (world.rank() == 1) {
// n13
        tents.async_insert(13, inf);
        // o14
        tents.async_insert(14, inf);
        // p15
        tents.async_insert(15, inf);
        // q16
        tents.async_insert(16, inf);
        // r17
        tents.async_insert(17, inf);
        // s18
        tents.async_insert(18, inf);
        // t19
        tents.async_insert(19, inf);
        // u20
        tents.async_insert(20, inf);
        // v21
        tents.async_insert(21, inf);
        // w22
        tents.async_insert(22, inf);
        // x23
        tents.async_insert(23, inf);
        // y24
        tents.async_insert(24, inf);

    }
    if (world.rank() == 3) {
        // z25
        tents.async_insert(25, inf);
        // aa26
        tents.async_insert(26, inf);
        // ab27
        tents.async_insert(27, inf);
        // ac28
        tents.async_insert(28, inf);
        // ad29
        tents.async_insert(29, inf);
        // ae30
        tents.async_insert(30, inf);
        // af31
        tents.async_insert(31, inf);
        // ag32
        tents.async_insert(32, inf);
        // ah33
        tents.async_insert(33, inf);
        // ai34
        tents.async_insert(34, inf);
        // aj35
        tents.async_insert(35, inf);
        // ak36
        tents.async_insert(36, inf);
    }
    world.barrier();

    // relax the source node ---------------------------------------------------------------------
    float upper_bound = 1;
    float old_upper_bound = 0;
    std::tuple<int, float> source = std::make_tuple(0, 0);
    relax(source, tents);
    world.barrier();

    // fill the bag based on the tents in the lookup table
    ygm::container::bag<std::tuple<int, float>> bbag(world);
    int i = 0;
    tents.for_all([&bbag, &i, &world, &upper_bound](auto &node, auto &tent) {
        if (i % 3 == world.rank()) {
           bbag.async_insert(std::make_tuple(node, tent));
           i++;
        }
    });
    world.barrier();

    int x = 0;
    //while (bbag.size() != 0) {
    while (x <  3){
        // find the local minimum on each rank ---------------------------------------------------------
        float min = inf;
        bbag.local_for_all([&min](std::tuple<int, float> &v){
            if (std::get<1>(v) < min) {
                min = std::get<1>(v);
            }
        });
        world.barrier();

        //std::cout << "The minimum on rank " << world.rank() << " is " << min << std::endl;

        // find the global minimum ---------------------------------------------------------------------
        auto global_min = world.all_reduce_min(min);

        std::cout << "the global min is " << global_min << std::endl;
        world.barrier();


        // get pairs in the current bucket ------------------------------------------------------------
        int delta = 3;
        old_upper_bound = upper_bound;
        upper_bound = (ceil(global_min / delta) + 1) * delta;
        std::cout << "upper bound on the bucket: " << upper_bound << std::endl;
        world.barrier();



        // do the light edge relaxations --------------------------------------------------------------
        bool is_light = true;
        bool more_lights_to_relax = false;
        std::vector<std::tuple<int, float>> current_pairs;

        // get the current bucket values
        bbag.local_for_all([&upper_bound, &world, &current_pairs](std::tuple<int, float> &v){
            if (std::get<1>(v) < upper_bound) {
                current_pairs.push_back(v);
            }
        });

        // now that the current bucket has been found, find requests
        world.barrier();
        std::vector<std::tuple<int, float>> requests = find_requests(world, current_pairs, is_light, delta);

        // once the requests for that iteration are made, check to see if there are any potential tent(v) that would be in the same bucket next time
        // this may or may not be actually updated, but it COULD BE...
        for (std::tuple<int, float> pair : requests) {
            if (std::get<1>(pair) < upper_bound) {
                more_lights_to_relax = true;
            }
        }
        // relax those requests
        world.barrier();
        relax_requests(world, requests, tents);
        world.barrier();

        // this could happen one or more times, as pairs can end up in the same bucket
        while (more_lights_to_relax) {
            more_lights_to_relax = false;
            std::cout << "repeated" << std::endl;
            current_pairs.clear();
            bbag.clear();
            world.barrier();

            // refill the bag
            tents.for_all([&bbag, &i, &world, &old_upper_bound](auto &node, auto &tent) {
                if (tent >= old_upper_bound) {
                    bbag.async_insert(std::make_tuple(node, tent));
                    //std::cout << "tent(" << node << ") = " << tent << std::endl;
                    i++;
                }
            });
            world.barrier();

            // get the current bucket values
            bbag.local_for_all([&upper_bound, &world, &current_pairs](std::tuple<int, float> &v){
                if (std::get<1>(v) < upper_bound) {
                    //std::cout << "{" << std::get<0>(v) << ", " << std::get<1>(v)  << "}" << std::endl;
                    current_pairs.push_back(v);
                }
            });

            // now that the current bucket has been found, find requests
            world.barrier();
            std::vector<std::tuple<int, float>> requests = find_requests(world, current_pairs, is_light, delta);


            // once the requests for that iteration are made, check to see if there are any potential tent(v) that would be in the same bucket next time
            // this may or may not be actually updated, but it COULD BE...
            for (std::tuple<int, float> pair : requests) {
                if (std::get<1>(pair) < upper_bound) {
                    more_lights_to_relax = true;
                }
            }

            // relax those requests
            world.barrier();
            relax_requests(world, requests, tents);
            world.barrier();
        }

        // do the heavy edge relaxations --------------------------------------------------------------
        is_light = false;

        requests = find_requests(world, current_pairs, false, delta);
        /*for (std::tuple<int, float> pair : requests) {
            std::cout << std::get<0>(pair) << ", " << std::get<1>(pair) << std::endl;
        }*/
        world.barrier();
        relax_requests(world, requests, tents);
        world.barrier();

        // now, clear the bag and reinsert it based on the upper bound
        bbag.clear();
        int i = 0;
        tents.for_all([&bbag, &i, &world, &upper_bound](auto &node, auto &tent) {
            if (tent >= upper_bound) {
                bbag.async_insert(std::make_tuple(node, tent));
                //std::cout << "tent(" << node << ") = " << tent << std::endl;
                i++;
            }
        });
        world.barrier();
        x++;
    }

    world.barrier();
    bbag.for_all([](std::tuple<int, float> pair) {
        std::cout << "node: " << std::get<0>(pair) << " -> " << std::get<1>(pair) << std::endl;
    });

    world.barrier();

    //std::cout << "the upper bound is " << upper_bound << std::endl;




    return 0;
}

std::vector<std::tuple<int, float>> find_requests(ygm::comm &world, std::vector<std::tuple<int, float>> tent_pairs, bool is_light, int delta) {
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

        adj_list.insert(adj_list.end(), {std::make_tuple(0, 3), std::make_tuple(14, 1), std::make_tuple(15, 2), std::make_tuple(16, 1), std::make_tuple(17, 1)});
        mat.async_insert(4, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(1, 3), std::make_tuple(3, 1)});
        mat.async_insert(5, adj_list);
        adj_list.clear();

        adj_list.insert(adj_list.end(), {std::make_tuple(1, 3), std::make_tuple(34, 1), std::make_tuple(19, 1), std::make_tuple(32, 2), std::make_tuple(33, 5)});
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

        adj_list.insert(adj_list.end(), {std::make_tuple(3, 1), std::make_tuple(31, 2), std::make_tuple(29, 3), std::make_tuple(30, 2)});
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
    if (world.rank() == 2) {
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


    std::vector<std::tuple<int, float>> requests;
    mat.for_all([&tent_pairs, &requests, &is_light, &delta](auto &row, auto &adj) {
        for (std::tuple<int, float> tent_pair : tent_pairs) {
            if (row == std::get<0>(tent_pair)) {
                for (int i = 0; i < adj.size(); ++i) {
                    // if the edge is the correct based on is_light, add it to the list
                    if ((std::get<1>(adj[i]) <= delta && is_light) || (std::get<1>(adj[i]) > delta && !is_light)) {
                        requests.push_back(std::make_tuple(std::get<0>(adj[i]), std::get<1>(tent_pair) + std::get<1>(adj[i])));
                    }
                }
            }
        }
    });
    world.barrier();

    //return requests;
    return requests;
}

void relax_requests(ygm::comm &world, std::vector<std::tuple<int, float>>& requests, ygm::container::map<int, float>& tents) {
    for (std::tuple<int, float> request : requests) {
        relax(request, tents);
    }
    world.barrier();
}

void relax(std::tuple<int, float>& request, ygm::container::map<int, float>& tents) {
    bool repeat = false;
    auto update_tent = [](auto node, auto &value, std::tuple<int, float> request) {
        // if the proposed tent from request is less than the current tent
        if (std::get<1>(request) < value) {
            value = std::get<1>(request);
            //std::cout << "tent(" << node << ") = " << value << std::endl;
        }
    };
    tents.async_visit(std::get<0>(request), update_tent, request);
}