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

#include "unordered_set"
#include <benchmark/benchmark.h>
#include <array>
#include <memory>
#include <random>
#include <vector>

namespace {

  struct my_data {
    size_t i;
    std::array<char, 256> data;
    explicit my_data(size_t i_) : i{i_} { std::iota(begin(data), end(data), 0); }
  };

  struct my_data_equal {
    bool operator()(const std::unique_ptr<my_data>& l, const std::unique_ptr<my_data>& r) const { return l->i == r->i; }
  };

  struct my_data_equal_transparent {
    using is_transparent = void;
    bool operator()(const std::unique_ptr<my_data>& l, const std::unique_ptr<my_data>& r) const { return l->i == r->i; }
    bool operator()(const std::unique_ptr<my_data>& l, int v) const { return l->i == v; }
    bool operator()(int v, const std::unique_ptr<my_data>& r) const { return v == r->i; }
  };

  struct my_data_hash {
    size_t operator()(const std::unique_ptr<my_data>& v) const { return std::hash<size_t>{}(v->i); }
  };

  struct my_data_hash_transparent {
    using transparent_key_equal = my_data_equal_transparent;  // KeyEqual to use
    size_t operator()(const std::unique_ptr<my_data>& v) const { return std::hash<size_t>{}(v->i); }
    size_t operator()(size_t v) const { return std::hash<size_t>{}(v); }
  };

  constexpr size_t item_count = 4096;

  using regular_set = std::unordered_set<std::unique_ptr<my_data>, my_data_hash, my_data_equal>;
  using heterogeneous_set = std::unordered_set<std::unique_ptr<my_data>, my_data_hash_transparent>;

  void bm_heterogeneous_set_count_regular(benchmark::State& state)
  {
    regular_set set;
    for(size_t i = 0; i < item_count; ++i) set.insert(std::make_unique<my_data>(i));

    for(auto _ : state)
      for(size_t i = 0; i < item_count; ++i) benchmark::DoNotOptimize(set.count(std::make_unique<my_data>(i)));
  }

  void bm_heterogeneous_set_count_heterogeneous(benchmark::State& state)
  {
    heterogeneous_set set;
    for(size_t i = 0; i < item_count; ++i) set.insert(std::make_unique<my_data>(i));

    for(auto _ : state)
      for(size_t i = 0; i < item_count; ++i) benchmark::DoNotOptimize(set.count(i));
  }

  BENCHMARK(bm_heterogeneous_set_count_regular);
  BENCHMARK(bm_heterogeneous_set_count_heterogeneous);

}  // namespace
