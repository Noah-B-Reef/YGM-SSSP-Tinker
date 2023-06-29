//
// Created by mfpate on 6/21/23.
//

#include "ygm/container/experimental/maptrix.hpp"

int main(int argc, char **argv) {
    ygm::comm world(&argc, &argv);

    ygm::container::experimental::maptrix<int, int> mat(world);
// _____________________________________________________________________
    if (world.rank() == 0) {
        mat.async_insert(0, 1, 3);
        mat.async_insert(0, 2, 3);
        mat.async_insert(0, 3, 5);
        mat.async_insert(0, 4, 3);
        // b1
        mat.async_insert(1, 0, 3);
        mat.async_insert(1, 5, 3);
        mat.async_insert(1, 6, 2);
        mat.async_insert(1, 7, 3);
        // c2
        mat.async_insert(2, 0, 3);
        mat.async_insert(2, 8, 2);
        mat.async_insert(2, 9, 5);
        mat.async_insert(2, 10, 4);
        // d3
        mat.async_insert(3, 0, 5);
        mat.async_insert(3, 5, 1);
        mat.async_insert(3, 11, 2);
        mat.async_insert(3, 12, 1);
        mat.async_insert(3, 13, 1);
        // e4
        mat.async_insert(4, 0, 3);
        mat.async_insert(4, 14, 1);
        mat.async_insert(4, 15, 2);
        mat.async_insert(4, 16, 1);
        mat.async_insert(4, 17, 1);
        // f5
        mat.async_insert(5, 1, 3);
        mat.async_insert(5, 3, 1);
        // g6
        mat.async_insert(6, 1, 2);
        mat.async_insert(6, 34, 1);
        mat.async_insert(6, 19, 1);
        mat.async_insert(6, 32, 2);
        mat.async_insert(6, 33, 5);
        // h7
        mat.async_insert(7, 1, 3);
        mat.async_insert(7, 27, 1);
        mat.async_insert(7, 28, 3);
        // i8
        mat.async_insert(8, 2, 2);
        mat.async_insert(8, 35, 4);
        mat.async_insert(8, 36, 4);
        // j9
        mat.async_insert(9, 2, 5);
        // k10
        mat.async_insert(10, 2, 4);
        mat.async_insert(10, 21, 3);
        mat.async_insert(10, 18, 3);
        mat.async_insert(10, 23, 2);
        mat.async_insert(10, 20, 1);
        // l11
        mat.async_insert(11, 3, 2);
        mat.async_insert(11, 31, 1);
        // m12
        mat.async_insert(12, 3, 1);
        // n13
        mat.async_insert(13, 3, 1);
        mat.async_insert(13, 31, 2);
        mat.async_insert(13, 29, 3);
        mat.async_insert(13, 30, 2);
        // o14
        mat.async_insert(14, 4, 2);
        mat.async_insert(14, 26, 1);
        // p15
        mat.async_insert(15, 4, 2);
        // q16
        mat.async_insert(16, 4, 3);
        mat.async_insert(16, 20, 6);
        mat.async_insert(16, 25, 4);
        // r17
        mat.async_insert(17, 4, 3);
        // s18
        mat.async_insert(18, 10, 3);
        // t19
        mat.async_insert(19, 6, 1);
        // u20
        mat.async_insert(20, 10, 1);
        mat.async_insert(20, 16, 6);
        mat.async_insert(20, 22, 3);
        mat.async_insert(20, 24, 2);
        // v21
        mat.async_insert(21, 10, 3);
        // w22
        mat.async_insert(22, 20, 3);
        // x23
        mat.async_insert(23, 10, 2);
        // y24
        mat.async_insert(24, 20, 2);
        // z25
        mat.async_insert(25, 16, 4);
        // aa26
        mat.async_insert(26, 14, 1);
        // ab27
        mat.async_insert(27, 7, 1);
        // ac28
        mat.async_insert(28, 7, 3);
        mat.async_insert(28, 33, 1);
        // ad29
        mat.async_insert(29, 13, 3);
        // ae30
        mat.async_insert(30, 13, 2);
        // af31
        mat.async_insert(31, 13, 1);
        mat.async_insert(31, 11, 1);
        // ag32
        mat.async_insert(32, 6, 2);
        // ah33
        mat.async_insert(33, 6, 1);
        mat.async_insert(33, 28, 1);
        // ai34
        mat.async_insert(34, 6, 1);
        // aj35
        mat.async_insert(35, 8, 4);
        // ak36
        mat.async_insert(36, 8, 4);

    }


    mat.for_all([&world](int &r, int &c, int &v) {
        std::cout << r << ", " << c << " -> " << v << std::endl;
        //std::cout << r << ", " << c << " -> " << v << std::endl;
        world.barrier();
    });
    world.barrier();

}