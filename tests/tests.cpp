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
#include <string>
#include <string_view>
#include <vector>
#include <random>
#include <benchmark/benchmark.h>


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
void test_2()
{
  std::vector<UnorderedMap> products;
  const auto id = "abcd";
  const auto userHash = string_hash{}(id);
  for(auto& users : products) {
//    users.find(id, userHash);
  }
}

constexpr size_t item_count = 1024;

struct test_data {
  std::vector<std::string> storage;
  std::vector<std::pair<std::string_view, int>> test_sequence;
};

test_data make_test_data(size_t base_str_length)
{
  test_data data;
  data.storage.reserve(item_count);
  data.test_sequence.reserve(item_count);
  for(size_t i=0; i<item_count; ++i) {
    data.storage.push_back(std::string(base_str_length, 'X') + std::to_string(i));
    data.test_sequence.emplace_back(data.storage.back(), i);
  }
  std::mt19937 g;
  std::shuffle(data.test_sequence.begin(), data.test_sequence.end(), g);
  return data;
}

using regular_map = std::unordered_map<std::string, int>;
using heterogeneous_map = std::unordered_map<std::string, int, string_hash, std::equal_to<>>;

void BM_find_regular(benchmark::State& state)
{
  auto data = make_test_data(state.range(0));
  regular_map map{data.test_sequence.begin(), data.test_sequence.end()};
  for(auto _ : state)
    for(const auto& el : data.test_sequence)
      map.find(std::string{el.first});
}

void BM_find_heterogeneous(benchmark::State& state)
{
  auto data = make_test_data(state.range(0));
  heterogeneous_map map{data.test_sequence.begin(), data.test_sequence.end()};
  for(auto _ : state)
    for(const auto& el : data.test_sequence)
      map.find(el.first);
}

}

BENCHMARK(BM_find_regular)->Arg(0)->Arg(128);
BENCHMARK(BM_find_heterogeneous)->Arg(0)->Arg(128);

BENCHMARK_MAIN();
