// Copyright 2019-2021 Lawrence Livermore National Security, LLC and other YGM
// Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: MIT

#undef NDEBUG
#include <string>
#include <ygm/comm.hpp>
#include <ygm/container/map.hpp>

int main(int argc, char **argv) {
  ygm::comm world(&argc, &argv);

  //
  // Test Rank 0 async_insert
  {
    ygm::container::multimap<std::string, std::string> smap(world);
    if (world.rank() == 0) {
      smap.async_insert("dog", "cat");
      smap.async_insert("apple", "orange");
      smap.async_insert("red", "green");
    }
    ASSERT_RELEASE(smap.count("dog") == 1);
    ASSERT_RELEASE(smap.count("apple") == 1);
    ASSERT_RELEASE(smap.count("red") == 1);
  }

  //
  // Test all ranks async_insert
  {
    ygm::container::multimap<std::string, std::string> smap(world);

    smap.async_insert("dog", "cat");
    smap.async_insert("apple", "orange");
    smap.async_insert("red", "green");

    ASSERT_RELEASE(smap.count("dog") == world.size());
    ASSERT_RELEASE(smap.count("apple") == world.size());
    ASSERT_RELEASE(smap.count("red") == world.size());
  }

  //
  // Test all ranks default & async_visit_if_exists
  {
    ygm::container::multimap<std::string, std::string> smap(world,
                                                            "default_string");
    smap.async_visit("dog", [](std::pair<const std::string, std::string> s) {
      ASSERT_RELEASE(s.first == "dog");
      ASSERT_RELEASE(s.second == "default_string");
    });
    smap.async_visit("cat", [](std::pair<const std::string, std::string> &s) {
      ASSERT_RELEASE(s.first == "cat");
      ASSERT_RELEASE(s.second == "default_string");
    });
    smap.async_visit_if_exists("red",
                               [](const auto &p) { ASSERT_RELEASE(false); });

    ASSERT_RELEASE(smap.count("dog") == 1);
    ASSERT_RELEASE(smap.count("cat") == 1);
    ASSERT_RELEASE(smap.count("red") == 0);

    ASSERT_RELEASE(smap.size() == 2);

    if (world.rank() == 0) {
      smap.async_erase("dog");
    }
    ASSERT_RELEASE(smap.count("dog") == 0);
    ASSERT_RELEASE(smap.size() == 1);
    smap.async_erase("cat");
    ASSERT_RELEASE(smap.count("cat") == 0);

    ASSERT_RELEASE(smap.size() == 0);
  }

  //
  // Test swap & async_set
  {
    ygm::container::multimap<std::string, std::string> smap(world);
    {
      ygm::container::multimap<std::string, std::string> smap2(world);
      smap2.async_insert("dog", "cat");
      smap2.async_insert("apple", "orange");
      smap2.async_insert("red", "green");
      smap2.swap(smap);
      ASSERT_RELEASE(smap2.size() == 0);
    }
    ASSERT_RELEASE(smap.size() == 3 * world.size());
    ASSERT_RELEASE(smap.count("dog") == world.size());
    ASSERT_RELEASE(smap.count("apple") == world.size());
    ASSERT_RELEASE(smap.count("red") == world.size());
    smap.async_insert("car", "truck");
    ASSERT_RELEASE(smap.size() == 4 * world.size());
    ASSERT_RELEASE(smap.count("car") == world.size());
  }

  //
  // Test local_get()
  {
    ygm::container::multimap<std::string, std::string> smap(world);
    smap.async_insert("foo", "barr");
    smap.async_insert("foo", "baz");
    smap.async_insert("foo", "qux");
    smap.async_insert("foo", "quux");
    world.barrier();
    auto values = smap.local_get("foo");
    if (smap.is_mine("foo")) {
      ASSERT_RELEASE(values.size() == 4 * world.size());
    } else {
      ASSERT_RELEASE(values.size() == 0);
    }
  }

  return 0;
}
