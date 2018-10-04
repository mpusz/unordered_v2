[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg?maxAge=3600)](https://raw.githubusercontent.com/mpusz/unordered_v2/master/LICENSE)
[![Travis CI](https://img.shields.io/travis/mpusz/unordered_v2/master.svg?label=Travis%20CI)](https://travis-ci.org/mpusz/unordered_v2)

# Improving unordered associative containers in the C++ Standard Library

That repository is a Proof Of Concept (POC) implementation of my papers intended to
improve the performance of unordered associative containers in the C++ Standard
Library.

Those papers are:
- [Heterogeneous lookup for unordered containers](https://mpusz.github.io/wg21_papers/papers/unordered_heterogeneous_lookup.html)


## Building instructions

POC implementation is based on [libc++ 5.0.0](https://libcxx.llvm.org). In order to
compile and run example code you have to install that version of libc++ on your PC.

Moreover it uses [Google Benchmark](https://github.com/google/benchmark) library to
implement time-sensitive tests. You may either download, build and install
that library by yourself or use `conan` package manager to that for you.


### Using `conan`

`conan` client can be downloaded from [Conan.io](https://conan.io).

Here are example instructions to download all dependencies, compile and run the build: 

```bash
mkdir build && cd build
conan install .. --build=missing <your_clang_profile_and_settings>
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-5.0 -DCMAKE_C_COMPILER=clang-5.0 -DCMAKE_CXX_FLAGS="-stdlib=libc++"
cmake --build .
ctest -VV
```
