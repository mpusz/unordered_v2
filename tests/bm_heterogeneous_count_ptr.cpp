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
    using is_transparent = void;
    bool operator()(const std::unique_ptr<my_data>& l, const std::unique_ptr<my_data>& r) const { return l == r; }
    bool operator()(const std::unique_ptr<my_data>& l, my_data* ptr) const { return l.get() == ptr; }
    bool operator()(my_data* ptr, const std::unique_ptr<my_data>& r) const { return ptr == r.get(); }
  };

  struct my_data_hash {
    using is_transparent = void;
    size_t operator()(const std::unique_ptr<my_data>& v) const { return std::hash<const my_data*>{}(v.get()); }
    size_t operator()(const my_data* ptr) const { return std::hash<const my_data*>{}(ptr); }
  };

  // based on https://stackoverflow.com/a/17853770
  class opt_out_deleter {
    bool delete_;

  public:
    explicit opt_out_deleter(bool do_delete = true) : delete_{do_delete} {}
    template<typename T>
    void operator()(T* p) const
    {
      if(delete_) delete p;
    }
  };

  template<typename T>
  using set_unique_ptr = std::unique_ptr<T, opt_out_deleter>;

  template<typename T>
  set_unique_ptr<T> make_find_ptr(T* raw)
  {
    return set_unique_ptr<T>{raw, opt_out_deleter{false}};
  }

  constexpr size_t item_count = 4096;

  struct test_data {
    std::vector<std::unique_ptr<my_data>> storage;
    std::vector<my_data*> test_sequence;
  };

  test_data make_test_data()
  {
    test_data data;
    data.storage.reserve(item_count);
    data.test_sequence.reserve(item_count);
    for(size_t i = 0; i < item_count; ++i) {
      data.storage.push_back(std::make_unique<my_data>(i));
      data.test_sequence.push_back(data.storage.back().get());
    }
    std::mt19937 g;
    std::shuffle(begin(data.test_sequence), end(data.test_sequence), g);
    return data;
  }

  using regular_set = std::unordered_set<set_unique_ptr<my_data>>;
  using regular_map = std::unordered_map<my_data*, std::unique_ptr<my_data>>;
  using heterogeneous_set = std::unordered_set<std::unique_ptr<my_data>, my_data_hash, my_data_equal>;

  void bm_heterogeneous_set_count_ptr_regular(benchmark::State& state)
  {
    auto data = make_test_data();
    regular_set set;
    for(auto& ptr : data.storage) set.emplace(ptr.release());

    for(auto _ : state)
      for(auto ptr : data.test_sequence) benchmark::DoNotOptimize(set.count(make_find_ptr(ptr)));
  }

  void bm_heterogeneous_map_count_ptr_regular(benchmark::State& state)
  {
    auto data = make_test_data();
    regular_map map;
    for(auto& ptr : data.storage) {
      auto p = ptr.release();
      map.emplace(p, p);
    }

    for(auto _ : state)
      for(auto ptr : data.test_sequence) benchmark::DoNotOptimize(map.count(ptr));
  }

  void bm_heterogeneous_set_count_ptr_heterogeneous(benchmark::State& state)
  {
    auto data = make_test_data();
    heterogeneous_set set;
    for(auto& ptr : data.storage) set.emplace(std::move(ptr));

    for(auto _ : state)
      for(auto ptr : data.test_sequence) benchmark::DoNotOptimize(set.count(ptr));
  }

  BENCHMARK(bm_heterogeneous_set_count_ptr_regular);
  BENCHMARK(bm_heterogeneous_map_count_ptr_regular);
  BENCHMARK(bm_heterogeneous_set_count_ptr_heterogeneous);

}  // namespace
