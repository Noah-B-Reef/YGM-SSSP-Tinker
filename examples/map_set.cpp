// Copyright 2019-2021 Lawrence Livermore National Security, LLC and other YGM
// Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: MIT

#include <string>
#include <ygm/container/map.hpp>
#include <ygm/container/set.hpp>

int main(int argc, char** argv) {
  ygm::comm world(&argc, &argv);

  ygm::container::set<std::string> str_set(world);
  ygm::container::map<std::string, std::string> str_map(world);

  if (world.rank() == 0) {
    str_set.async_insert("four");
    str_set.async_insert("score");
    str_set.async_insert("seven");
    str_set.async_insert("years");
    str_set.async_insert("ago");

    str_map.async_set("apple", "orange");
    str_map.async_set("car", "truck");
    str_map.async_set("strawberry", "raspberry");
  }

  str_set.all_for_all(
      [](auto k) { std::cout << "str_set: " << k << std::endl; });

  str_map.for_all([](auto kv) {
    std::cout << "str_map: " << kv.first << " -> " << kv.second << std::endl;
  });
  return 0;
}