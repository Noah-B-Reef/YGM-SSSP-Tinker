#define _GLIBCXX_USE_CXX11_ABI 0
#include <ygm/comm.hpp>
#include <ygm/container/bag.hpp>
#include "ygm/container/experimental/maptrix.hpp"
#include <tuple>
#include <cmath>
#include <limits>
#include "adjacency.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ygm/comm.hpp>
#include <ygm/detail/comm_impl.hpp>
#include <ygm/container/set.hpp>
#include <istream>
#include <limits>
#include <tuple>
#include <vector>
#include <string>
#include <ygm/container/map.hpp>
using namespace std;



bool isEmpty(std::vector<ygm::container::set<int>*> &buckets);


int main(int argc, char *argv[]) {

    // Intialization
    const float INF = std::numeric_limits<float>::infinity();
    float delta;
    int num_buckets;    // N = ceil(largest weight/Delta)
    float max_weight;
    int max_degree;
    string path = "/home/noahr/YGM_SSSP_Tinker/examples/data.csv";


    ygm::comm world(&argc, &argv);
    ygm::container::map<size_t, adj_list> mat(world);
    std::vector<ygm::container::set<int>*> buckets;
    ygm::container::set<int> sset(world);
    buckets.push_back(&sset);

    // load in graph
    getGraph(world, mat, max_weight, path);

    // set source node

   
        mat.async_visit(0, [](auto k, auto &v)
        {
            v.tent = 0;
        });

    writer(world, mat);
    // get num of buckets

    // use user defined num of buckets
    if (argc > 1)
    {
        num_buckets = std::atoi(argv[1]);
        delta = std::atoi(argv[2]);

    }
    // compute necessary num of buckets
    else{

        int degree = 0;

        mat.for_all([&degree](auto k, auto v)
        {
            if (v.edges.size() > degree)
            {
                degree = v.edges.size()-1;
            }
        });

        max_degree = world.all_reduce_max(degree);

        delta = 1.0/max_degree;
        
        num_buckets = (int)ceil(max_weight/delta);

    }
    


    // relax the source node
    buckets[0]->async_insert(0);

    world.barrier();

    int curr_bucket = 0;
    int count = 0;

    /*
    
    // consume the bucket
    while(!isEmpty(buckets) && count < 3)
    {
        // print out curr_bucket
        
        int request = 1; // 1 for light requests, 0 for heavy


        static auto thing = [&mat, &request, &delta](auto k, auto v){

                std::vector<std::tuple<int,float>> row = v.edges;
                float tent = v.tent;

                // iterate through adjacency row
                for (int i = 0; i < row.size(); i++)
                {
                    float curr_cost = std::get<1>(row[i]);
                    int node = std::get<0>(row[i]);

                    // if heavy request
                    if (request == 0)
                    {
                        float potential = tent + curr_cost;

                        // check if node meets heavy condition
                        if (curr_cost > delta)
                        {
                            auto update = [](auto k, auto &v, float potential)
                            {
                                float tent = v.tent;

                                if (potential < tent)
                                {
                                    v.tent = potential;
                                }
                            };
                            mat.async_visit(node, update, potential);
                        }
                    }

                    // else light request
                    else
                    {

                        float potential = tent + curr_cost;
                        std::cout << curr_cost << " <= " << delta << "-->" << potential << std::endl;
                        // check if node meets light condition
                        if (curr_cost <= delta)
                        {
                            auto update = [](auto k, auto &v, float potential)
                            {
                                std::cout << "I made it here!" << std::endl;

                                float tent = v.tent;

                                if (potential < tent)
                                {
                                    v.tent = potential;
                                    std::cout << "(" << k << "," << v.tent << ")" << std::endl;
                                }
                            };
                            mat.async_visit(node, update, potential);
                        }
                    }

                }
            };

        // get current requests for each iteration
        auto get_requests = [&mat, &request, &delta](auto v){
            // Erase node
            // TODO
            mat.async_visit(v,[](auto k, auto v){
                thing(k,v);
            });
        };

        buckets[curr_bucket]->for_all(get_requests);

        count++;
    }
    */
}

// Check if ALL buckets are empty
bool isEmpty(std::vector<ygm::container::set<int>*> &buckets)
{
    for (int i = 0; i < buckets.size(); i++)
    {
        if (buckets[i]->size() != 0)
        {
            return false;
        }
    }

    return true;
}