//
// Created by mfpate on 7/3/23.
//

#include <ygm/comm.hpp>
#include <ygm/container/array.hpp>
#include <ygm/container/set.hpp>
#include <ygm/container/map.hpp>
#include <cmath>
#include <limits>
#include <tuple>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ygm/comm.hpp>
#include <ygm/detail/comm_impl.hpp>
#include <ygm/container/bag.hpp>
#include <istream>
#include <limits>
#include <tuple>
#include <vector>
#include <string>
#include <ygm/container/map.hpp>
using namespace std;

struct adj_list {
    std::vector<std::tuple<int, float>> edges;
    float tent;

    template<class Archive>
    void serialize(Archive & ar) {
        ar(edges, tent);
    }
};

void getGraph(ygm::comm &world, ygm::container::map<int, adj_list> &mat);

int main(int argc, char **argv) {
    ygm::comm world(&argc, &argv);

    // here is the lookup map for vertices and their best tent values/adj list (as a struct)
    ygm::container::map<int, adj_list> map(world);
    getGraph(world, map);

    // the array of sets of vertices
    std::vector<ygm::container::set<int>*> buckets;

    // THIS WILL NEED TO BE CHANGED!!
    const float max_cost = 21; // the maximum shortest path weight
    const static int delta = 3;
    int num_buckets = ceil(max_cost / delta) + 2;

    // add the sets to the vector -------------------------------------------------------------------------------------
    // TODO: update this so that it is not hard coded

    ygm::container::set<int> bucket0(world);
    buckets.push_back(&bucket0);

    ygm::container::set<int> bucket1(world);
    buckets.push_back(&bucket1);

    ygm::container::set<int> bucket2(world);
    buckets.push_back(&bucket2);

    ygm::container::set<int> bucket3(world);
    buckets.push_back(&bucket3);

    ygm::container::set<int> bucket4(world);
    buckets.push_back(&bucket4);

    ygm::container::set<int> bucket5(world);
    buckets.push_back(&bucket5);


    ygm::container::set<int> bucket6(world);
    buckets.push_back(&bucket6);

    ygm::container::set<int> bucket7(world);
    buckets.push_back(&bucket7);

    ygm::container::set<int> bucket8(world);
    buckets.push_back(&bucket8);


    int idx = 0;
    // complete a source relaxation --------------------------------------------------------------------------------------
    // relax the source
    map.async_visit(0, [](auto &source, auto &source_info) {
        source_info.tent = 0;
    });
    // insert the source into the first bucket
    buckets[0]->async_insert(0);


    // duplicate the current bucket -----------------------------------------------------------
    ygm::container::set<int> bucket_copy(world);

    static auto fill_bucket_copy_lambda = [&bucket_copy](const auto &vertex) {
        // if the vertex is not already in the copy bucket, add it
        bucket_copy.async_insert(vertex);
    };

    buckets[idx]->for_all([](const int &vertex) {
       fill_bucket_copy_lambda(vertex);
    });

    // process the current bucket ---------------------------------------------------------------
    static auto add_vertex_to_bucket_lambda = [&buckets](auto &vertex, auto &idx) {
        buckets[idx]->async_insert(vertex);
    };

    // For a given node (this is the tail of an edge from a node in the current bucket),
    // update the tent value if the potential is better
    static auto relax_requests_lambda = [&map](auto &vertex, auto &potential_tent) {
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
    auto remove_from_bucket_lambda = [&buckets, &idx](const auto &vertex) {
        buckets[idx]->async_erase(vertex);
    };

    // for all vertices in the current bucket:
    // first, remove them from the bucket
    // then, go to that row in the map, walk through its adjacency list and relax all requests
    while (idx < num_buckets) {
        // check to see if there is even anything in the current bucket
        while (buckets[idx]->size() > 0) {
            buckets[idx]->for_all([&map, &remove_from_bucket_lambda](int vertex) {
                // add all vertices in the current bucket to the copy
                fill_bucket_copy_lambda(vertex);

                // remove the current vertex from the current bucket
                remove_from_bucket_lambda(vertex);

                // go to that row in the map and relax requests
                map.async_visit(vertex, [](const auto &head, adj_list &head_info) {
                    for (std::tuple<int, float> edge : head_info.edges) {
                        if (std::get<1>(edge) <= delta) {
                            float potential_tent = head_info.tent + std::get<1>(edge);
                            relax_requests_lambda(std::get<0>(edge), potential_tent);
                        }
                    }
                });
            });
            world.barrier();
        }

        // do the heavy relaxations (only one round) ---------------------------------------------------------------------
        bucket_copy.for_all([&map](int vertex) {
            // go to that row in the map and relax requests
            map.async_visit(vertex, [](const auto &head, adj_list &head_info) {
                for (std::tuple<int, float> edge : head_info.edges) {
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

    // print out the final distances from the source for each node
    map.for_all([](auto &vertex, adj_list &vertex_info) {
        std::cout << "tent(" << vertex << ") = " << vertex_info.tent << std::endl;
    });

}


// HELPER FUNCTIONS ------------------------------------------------------------------------------------------------------
// load in graph from data.csv
void getGraph(ygm::comm &world, ygm::container::map<int, adj_list> &mat) {
    float Inf = std::numeric_limits<float>::infinity();

    // file pointer
    ifstream fin;

    // open data.csv
    fin.open("/home/molliep/ygm/examples/data.csv");

    std::vector <string> row;
    std::vector<std::tuple<int, float>> adj;
    string line, word, temp;

    // keep track of current node's adjacency list
    int curr_node = 0;

    // skip first line
    getline(fin, line);

    while (getline(fin, line)) {
        // breaking words
        std::stringstream s(line);

        // read column data
        while(std::getline(s, word,char(','))) {
            row.push_back(word);
        }

        // load adjacency row into matrix
        if (std::stoi(row[0]) != curr_node) {
            if (world.rank() == curr_node % world.size()) {
                adj_list insert;
                insert.edges = adj;
                insert.tent = Inf;
                        //{adj, Inf};
                mat.async_insert(curr_node, insert);
            }
            adj.clear();
            curr_node++;
        }

        adj.push_back(std::make_tuple(std::stoi(row[1]), std::stof(row[2])));
        row.clear();
    }

    if (world.rank() == curr_node % world.size()) {
        adj_list insert = {adj, Inf};
        mat.async_insert(curr_node, insert);
    }

    world.barrier();
    fin.close();
}