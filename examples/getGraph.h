#define _GLIBCXX_USE_CXX11_ABI 0
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

namespace gg{

class getGraph
{

public:
    // load in graph from data.csv
    void getGraph(ygm::comm &world,
    ygm::container::map<int,std::vector<std::tuple<int,float>>> &mat,
    ygm::container::map<int, <float, bool>> &tentMat)
    {
        float Inf = std::numeric_limits<float>::infinity();


        // file pointer
        ifstream fin;

        // open data.csv
        fin.open("/home/molliep/ygm/examples/data.csv");


        std::vector<string> row;
        std::vector<std::tuple<int,float>> adj;
        
        string line,word,temp;

        // keep track of current node's adjacency list
        int curr_node = 0;

        // skip first line
        getline(fin,line);

        while (getline(fin, line))
        {
                // breaking words
                std::stringstream s(line);

                
                    // read column data
                    while(std::getline(s,word,char(',')))
                    {
                        row.push_back(word);
                    }
                


                // load adjency row into matrix
                if (std::stoi(row[0]) != curr_node)
                {
                    
                    if (world.rank() == curr_node % world.size())
                    {
                        tentMat.async_insert(curr_node, Inf, true);
                        mat.async_insert(curr_node, adj);
                    }

                    adj.clear();
                    curr_node++;
                }

                adj.push_back(std::make_tuple(std::stoi(row[1]),std::stof(row[2])));
                row.clear();
            }

            if (world.rank() == curr_node % world.size())
            {
                tentMat.async_insert(curr_node, Inf, true);
            }
            
            world.barrier(); 
            fin.close();

        }
    };
}
