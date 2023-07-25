#define _GLIBCXX_USE_CXX11_ABI 0
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ygm/comm.hpp>
#include <ygm/detail/comm_impl.hpp>
#include <ygm/container/set.hpp>
#include <istream>
#include <limits>
#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <ygm/container/map.hpp>

using namespace std;
struct adj_list {

    std::map<std::size_t, float> edges;

    float tent;

    template<class Archive>

    void serialize(Archive & ar)
    {
        ar(edges, tent);
    }

};


// load in graph from data.csv
void getGraph(ygm::comm &world, ygm::container::map<std::size_t, adj_list> &mat, float &max_weight, std::string path) {

    float Inf = std::numeric_limits<float>::infinity();
    int max = 0;

    // file pointer
    std::ifstream fin;

    // open data.csv
    fin.open(path);

    std::vector <std::string> row;
    std::map<std::size_t, float> adj;
    //std::vector <std::tuple<std::size_t, float>> adj;
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

        // load adjency row into matrix
        if (std::stoi(row[0]) != curr_node) {
            if (world.rank() == curr_node % world.size()) {
                adj_list insert = {adj, Inf};
                mat.async_insert(curr_node, insert);

                // update maxs
                if (max < std::stoi(row[2]))
                {
                    max = std::stoi(row[2]);
                }
            }

            adj.clear();
            curr_node++;
        }


        //adj.push_back (std::make_tuple(std::stoi(row[1]), std::stof(row[2])));
        adj.insert({std::stoi(row[1]), std::stof(row[2])});
    }

    if (world.rank() == curr_node % world.size()) {
        adj_list insert = {adj, Inf};
        mat.async_insert(curr_node, insert);
    }

    world.barrier();

    // get max across all ranks
    max_weight = world.all_reduce_max(max);

    fin.close();

}


