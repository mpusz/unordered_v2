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

#include "experimental/unordered_map"
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>

//#include "hopscotch_map.h"
//#include "inplace_string.h"


using namespace std::literals;

namespace {

 struct string_hash {
   using hash_type = std::hash<std::string_view>;

   size_t operator()(const std::string_view txt) const { return hash_type{}(txt); }
   size_t operator()(const std::string& txt) const { return hash_type{}(txt); }
   size_t operator()(const char* txt) const { return hash_type{}(txt); }

//    template<size_t MAX_SIZE>
//    size_t operator()(const mp::inplace_string<MAX_SIZE>& txt) const
//    {
//      return std::hash<std::string_view>{}(txt);
//    }

  };

  struct string_equal {
    using is_transparent = void;

    template<typename L, typename R>
    constexpr bool operator()(const L& lhs, const R& rhs) const
    {
      return std::string_view{lhs} == std::string_view{rhs};
    }
  };

//  using user_id = mp::inplace_string<16>;
//  using user_map = tsl::hopscotch_map<user_id, int, string_hash, string_equal>;
//  using users = std::unordered_map<id, int, string_hash, string_equal>;
}
//
//TEST(unordered, NoNeedToRecalculateHash)
//{
//  std::vector<user_map> products;
//  const auto id = "abcd";
//  const auto userHash = string_hash{}(id);
//  for(auto& users : products) {
//    users.find(id, userHash);
//  }
//}


template<typename UnorderedMap>
void test_1()
{
  UnorderedMap map;
  map.find("abcd");      // const char *
  map.find("abcd"s);     // std::string
//  map.find("abcd"sv);    // std::string_view
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


int main()
{
  test_1<std::unordered_map<std::string, int>>();
  test_1<std::experimental::unordered_map<std::string, int>>();

  test_2<std::unordered_map<std::string, int>>();
  test_2<std::experimental::unordered_map<std::string, int>>();
}
