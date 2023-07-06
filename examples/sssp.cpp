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

// find_requests(v', light/heavy)
// relax_requests(requests)
// relax(w, v)x

struct adj_list {
    std::vector<std::tuple<int, float>> cost;
    float tent;

    template<class Archive>
    void serialize(Archive & ar)
    {
        ar(cost, tent);
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
    const float max_cost = 11; // the maximum shortest path weight
    const int delta = 3;
    int num_buckets = ceil(max_cost / delta) + 1;

    // add the sets to the vector

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

    //std::cout << buckets.size() << std::endl;
    // relax (source, 0)
    // while not all the buckets are empty
    int idx = 0;
    buckets[0]->async_insert(0);
    // relax the source
    map.for_all([](auto &vertex, auto &vertex_info) {
        if (vertex == 0) {
            vertex_info.tent = 0;
        }
    });


    static auto relax_requests_lambda = [&map](auto &vertex, auto &potential_tent) {
        map.async_visit(vertex, [](auto &node, adj_list &node_info, auto &potential_tent) {
            if (potential_tent < node_info.tent) {
               node_info.tent = potential_tent;
            }
            std::cout << "vertex = " << node << ", tent = " << node_info.tent << std::endl;
        }, potential_tent);
    };

    /*static auto collect_requests_lambda = [](const auto &vertex, adj_list &vertex_info) {
        std::cout << "vertex = " << vertex << ", tent = " << vertex_info.tent << std::endl;
        for (std::tuple<int, float> edge : vertex_info.cost) {
            std::cout << "{" << std::get<0>(edge) << ", " << std::get<1>(edge) << "}" << std::endl;
            //map.async_visit(std::get<0>(edge), relax_requests_lambda, vertex_info.tent + std::get<1>(edge));
        }
    };*/


    //while (pool.size() != 0) {
    buckets[0]->for_all([&map, &world](int vertex) {
        map.async_visit(vertex, [&map, &world](const auto &head, adj_list &head_info) {
            for (std::tuple<int, float> edge : head_info.cost) {
                float potential_tent = head_info.tent + std::get<1>(edge);
                relax_requests_lambda(std::get<0>(edge), potential_tent);
            }
        });

        /*for (std::tuple<int, float> request : requests) {
            std::cout << std::get<0>(request) << ", " << std::get<1>(request) << std::endl;
        }*/
       // for each node in the current bucket
       // visit that row in the map, collect the tent(v), c(e)
       // visit each w and compare the new tent value
       // if can be updated, update, and place node in a new bucket
    });
        // CONSUME_ALL vertices in the current bucket gather requests -> package up the tent(v), w and cost
        // go to the w row, compare the potential new tent(v), update if necessary
        // find the minimum non-empty bucket
        // while the bucket is not empty
        //while (pool[0].size() != 0) {
            // DO THIS AS A CONSUME:
                // find the light requests
                // remember deleted nodes
                // empty current bucket
                // relax requests (light edges)
        //}
        // find the heavy requests
        // relax the heavy requests
    //}
}

void find_requests() {
    // given a set of nodes, and the type of edges (light/heavy)
    // return all the (w, tent(v) + c_vw) of that edge type
}

void relax_requests() {
    // for each (w, x) pair in the set of requests, relax each request
}

void relax() {
    // if x < tent(w):
        // B[floor(tent(w)/delta)] = B[floor(tent(w)/delta)] \ {w}
        // B[floor(x/delta)] = B[floor(x/delta)] U {w}
        // tent(w) = x
}

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
                insert.cost = adj;
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
    world.barrier();
    fin.close();
}