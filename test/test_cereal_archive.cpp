// Copyright 2019-2021 Lawrence Livermore National Security, LLC and other YGM
// Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: MIT

#undef NDEBUG
#include <ygm/detail/ygm_cereal_archive.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

int main() {
  std::vector<std::string> vec_sentences = {
      "Four score and seven years ago",
      "our fathers brought forth on this continent",
      "a new nation conceived in liberty"};

  std::vector<char> buffer;
  {
    cereal::YGMOutputArchive archive(buffer);
    for (const auto& s : vec_sentences) { archive(s); }
  }

  {
    std::ifstream is("out.cereal", std::ios::binary);
    cereal::YGMInputArchive archive(buffer.data(), buffer.size());

    std::vector<std::string> out_sentences;
    while (!archive.empty()) {
      std::string tmp;
      archive(tmp);
      // std::cout << tmp << std::endl;
      out_sentences.push_back(tmp);
    }
    ASSERT_RELEASE(vec_sentences == out_sentences);
  }

  return 0;
}
