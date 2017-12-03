// The MIT License (MIT)
//
// Copyright (c) 2017 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "unordered_map"
#include "unordered_set"
#include <benchmark/benchmark.h>
#include <string>
#include <string_view>
#include <vector>

using namespace std::literals;

namespace {

struct string_hash {
  using is_transparent = void;                        // I confirm I know what I am doing
  using hash_type = std::hash<std::string_view>;      // helper local type
  size_t operator()(const std::string_view txt) const { return hash_type{}(txt); }
  size_t operator()(const std::string& txt) const     { return hash_type{}(txt); }
  size_t operator()(const char* txt) const            { return hash_type{}(txt); }
};


template<typename UnorderedMap>
void test_1()
{
  UnorderedMap map;
  map.find("abcd");      // const char *
  map.find("abcd"s);     // std::string
  map.find("abcd"sv);    // std::string_view
//  map.find(user_id{"abcd"});  // mp::inplace_string
}

template<typename UnorderedMap>
void test_2()
{
  std::vector<UnorderedMap> products;
  const auto id = "abcd";
  const auto userHash = string_hash{}(id);
  for(auto& users : products) {
//    users.find(id, userHash);
  }
}


static void BM_Find(benchmark::State& state)
{
  std::unordered_map<std::string, int, string_hash, std::equal_to<>> map;
  for (auto _ : state)
    map.find("123"sv);
}
BENCHMARK(BM_Find);

}

BENCHMARK_MAIN();
