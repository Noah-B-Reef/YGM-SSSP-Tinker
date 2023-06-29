//
// Created by mfpate on 6/23/23.
//

#include "ygm/container/map.hpp"
#include <ygm/comm.hpp>
#include <vector>
#include <tuple>

int main(int argc, char **argv) {
    ygm::comm world(&argc, &argv);

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

    // -------------------------------------------------------------------------
    world.barrier();


    mat.for_all([](auto &row, auto &adj) {
        std::cout << row << " -> " << std::get<0>(adj[0]) << ", " << std::get<1>(adj[0]) << std::endl;
    });
    world.barrier();
}