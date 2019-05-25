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
#include <benchmark/benchmark.h>
#include <array>
#include <random>
#include <string>
#include <string_view>
#include <vector>
#include <mutex>


using namespace std::literals;

namespace {

  template<typename Key, typename Hash>
  struct hash_cache {
    inline static std::pair<Key, std::size_t> cache;
    size_t operator()(const Key& k) const
    {
      std::size_t val{};
      if (k != cache.first) {
        cache.first = k;
        cache.second = Hash()(k);
      }
      val = cache.second;
      return val;
    }
  };

  template<typename Key, typename Hash>
  struct hash_cache_sync {
    inline static std::mutex m;
    inline static std::pair<Key, std::size_t> cache;
    size_t operator()(const Key& k) const
    {
      std::size_t val{};
      {
        std::scoped_lock lock(m);
        if (k != cache.first) {
          cache.first = k;
          cache.second = Hash()(k);
        }
        val = cache.second;
      }
      return val;
    }
  };

  constexpr size_t item_count = 128;
  constexpr size_t array_size = 32;

  struct test_data {
    std::vector<std::string> storage;
    std::vector<std::pair<std::string, int>> test_sequence;
  };

  test_data make_test_data(size_t base_str_length)
  {
    test_data data;
    data.storage.reserve(item_count);
    data.test_sequence.reserve(item_count);
    for(size_t i = 0; i < item_count; ++i) {
      data.storage.push_back(std::string(base_str_length, 'X') + std::to_string(i));
      data.test_sequence.emplace_back(data.storage.back(), i);
    }
    std::mt19937 g;
    std::shuffle(data.test_sequence.begin(), data.test_sequence.end(), g);
    return data;
  }

  void bm_precalculated_map_find(benchmark::State& state)
  {
    auto data = make_test_data(state.range(0));
    std::array<std::unordered_map<std::string, int>, array_size> maps;
    for(auto& m : maps) m.insert(data.test_sequence.begin(), data.test_sequence.end());

    for(auto _ : state)
      for(const auto& el : data.test_sequence)
        for(auto& m : maps) benchmark::DoNotOptimize(m.find(el.first));
  }

  void bm_precalculated_map_find_cache(benchmark::State& state)
  {
    auto data = make_test_data(state.range(0));
    std::array<std::unordered_map<std::string, int, hash_cache<std::string, std::hash<std::string>>>, array_size> maps;
    for(auto& m : maps) m.insert(data.test_sequence.begin(), data.test_sequence.end());

    for(auto _ : state)
      for(const auto& el : data.test_sequence)
        for(auto& m : maps) benchmark::DoNotOptimize(m.find(el.first));
  }

  void bm_precalculated_map_find_cache_sync(benchmark::State& state)
  {
    auto data = make_test_data(state.range(0));
    std::array<std::unordered_map<std::string, int, hash_cache_sync<std::string, std::hash<std::string>>>, array_size> maps;
    for(auto& m : maps) m.insert(data.test_sequence.begin(), data.test_sequence.end());

    for(auto _ : state)
      for(const auto& el : data.test_sequence)
        for(auto& m : maps) benchmark::DoNotOptimize(m.find(el.first));
  }

  void bm_precalculated_map_find_precalc(benchmark::State& state)
  {
    auto data = make_test_data(state.range(0));
    std::array<std::unordered_map<std::string, int>, array_size> maps;
    for(auto& m : maps) m.insert(data.test_sequence.begin(), data.test_sequence.end());

    for(auto _ : state)
      for(const auto& el : data.test_sequence) {
        const auto hash = maps.front().hash_token(el.first);
        for(auto& m : maps) benchmark::DoNotOptimize(m.find(el.first, hash));
      }
  }

  BENCHMARK(bm_precalculated_map_find)->Arg(0)->Arg(128);
  BENCHMARK(bm_precalculated_map_find_cache)->Arg(0)->Arg(128);
  BENCHMARK(bm_precalculated_map_find_cache_sync)->Arg(0)->Arg(128);
  BENCHMARK(bm_precalculated_map_find_precalc)->Arg(0)->Arg(128);

}  // namespace
