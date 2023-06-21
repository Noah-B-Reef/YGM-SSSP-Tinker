
#include <stdio.h> 
#include <ygm/comm.hpp>
#include <ygm/detail/comm_impl.hpp>
#include <ygm/container/bag.hpp>
#include <ygm/container/array.hpp>
#include <iostream>
#include <limits>
#include <tuple>
#include <vector>

int main(int argc, char **argv)
{
    // Delta-Stepping Example Graph

    // Offset Vector
    int offset[] = {

        0,
        4,
        8,
        12,
        17,
        22,
        24,
        29,
        32,
        35,
        36,
        41,
        43,
        44,
        48,
        50,
        51,
        54,
        55,
        56,
        57,
        61,
        62,
        63,
        64,
        65,
        66,
        67,
        68,
        70,
        71,
        72,
        74,
        75,
        78,
        79,
        80
    };

    // column index vector
    int c_index[] = {
        2,3,4,5,                // A
        1,6,7,8,                // B
        1,9,10,11,              // C
        1,6,12,13,14,           // D
        1,15,16,17,18,          // E
        2,4,                    // F
        2,20,33,34,35,          // G
        2,28,29,                // H
        3,36,37,                // I
        3,                      // J
        3,19,21,22,24,          // K
        4,32,                   // L
        4,                      // M
        4,30,31,32,             // N
        18,27,                  // O
        5,                      // P
        5,21,26,                // Q
        5,15,                   // R
        11,                     // S
        7,                      // T
        11,17,23,25,            // U
        11,                     // V
        21,                     // W
        11,                     // X
        21,                     // Y
        17,                     // Z
        15,                     // AA
        8,                      // AB
        8,34,                   // AC
        14,                     // AD
        14,                     // AE
        12,14,                  // AF
        7,                      // AG
        7,29,                   // AH
        7,                      // AI
        9,                      // AJ
        9                       // AK



    };

    // weights vector
    float weights[] = {
        3,3,5,3,        // A's edge weights
        3,3,2,3,        // B's edge weights
        3,2,5,4,        // C's edge weights
        5,1,2,1,1,      // D's edge weights
        3,2,2,3,3,      // E's edge weights
        3,1,            // F's edge weights
        2,1,2,1,5,      // G's edge weights
        3,1,3,          // H's edge weights
        2,4,4,          // I's edge weights
        5,              // J's edge weights
        4,3,1,3,2,      // K's edge weights
        2,1,            // L's edge weights
        1,              // M's edge weights
        1,3,2,1,        // N's edge weights
        2,1,            // O's edge weights
        2,              // P's edge weights
        3,6,4,          // Q's edge weights
        3,              // R's edge weights
        3,              // S's edge weights
        1,              // T's edge weights
        1,6,3,2,        // U's edge weights
        3,              // V's edge weights
        3,              // W's edge weights
        2,              // X's edge weights
        2,              // Y's edge weights
        4,              // Z's edge weights
        1,              // AA's edge weights
        1,              // AB's edge weights
        3,1,            // AC's edge weights
        3,              // AD's edge weights
        2,              // AE's edge weights
        1,1,            // AF's edge weights
        2,              // AG's edge weights
        1,1,            // AH's edge weights
        5,              // AI's edge weights
        4,              // AJ's edge weights
        4,              // AK's edge weights

    };



   // Generate initial tenative weights
   int size = sizeof(offset)/sizeof(int);
   const float Inf = std::numeric_limits<float>::infinity();
   double tenative[size];
   std::fill_n(tenative, size ,Inf);

   // Letting node A be the source node
   tenative[0] = 0;

   ygm::comm world(&argc, &argv);
   ygm::container::bag<std::tuple<int,float>> bbag(world);

    for(int i = world.rank(); i < world.rank() + 1; i++)
    {
        std::tuple<int,float> insert = std::make_tuple(i+1, tenative[i]);
        bbag.async_insert(insert);
    }

     /*
    for (int i = 0; i < world.size(); i++)
    {
        if (i == world.rank()){
            std::cout<<"Rank " << i << std::endl;
            bbag.local_for_all([](std::tuple<int,float> &s){std::cout <<"("<< std::get<0>(s) << "," << std::get<1>(s) << ")" << std::endl;});
            std::cout << std::endl;
        }
        world.barrier();
    }
    */
    

    /* 
    std::cout<<"Rank " << world.rank() << " size: " << bbag.local_size()  << std::endl;
    bbag.local_for_all([world](std::tuple<int,float> &s){std::cout <<"("<< std::get<0>(s) << "," << std::get<1>(s) << ", " << world.rank() << ")" << std::endl;});
    world.barrier();
    std::cout << std::endl;
    world.barrier();
    */

    // Convert vector to a sorted 
    //auto min = world.all_reduce_min(temp);

    for (int i = 0; i < world.size(); i++)
    {
        if(world.rank() == 0)
        {
            bbag.async_insert(i);
        }

        world.barrier();
        bbag.local_for_all([](){std:: cout << "Hi I am rank " << world.rank() << "and the number of my elements are " << bbag.local_size() << endl;});
        world.barrier();
    }

  return 0;



};
