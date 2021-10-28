#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <iostream>
#include <chrono>
#include <x86intrin.h>

struct Benchmark{
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    Benchmark(){
        start = std::chrono::steady_clock::now();
    }
    ~Benchmark(){
        end = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout<<diff.count()<<" ms\n";
    }
};

#endif 
