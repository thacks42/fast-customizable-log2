#include <iostream>
#include <cmath>
#include <cstring>
#include <vector>
#include <array>
#include <random>
#include "customizable_log2.hpp"
#include "benchmark.hpp"


std::vector<float> gen_random_vector(size_t N){
    std::vector<float> result;
    result.reserve(N);
    std::minstd_rand gen(42);
    std::uniform_real_distribution<float> dist(1.0f, 100.0f);
    for(size_t i = 0; i < N; i++){
        result.push_back(dist(gen));
    }
    return result;
}

std::vector<float> gen_range_vector(float min, float max, size_t N){
    std::vector<float> result;
    result.reserve(N);
    for(size_t i = 0; i < N; i++){
        float x = (max-min)/N * i + min;
        result.push_back(x);
    }
    return result;
}

float __attribute__((noinline)) benchmark_libmath(const std::vector<float>& vec){
    Benchmark b;
    float res = 0.0f;
    for(auto f : vec) res += std::log2(f);
    return res;
}

template<size_t bit_size>
float  __attribute__((noinline)) benchmark_approx_log_cubic(const std::vector<float>& vec){
    Benchmark b;
    float res = 0.0f;
    for(auto f : vec) res += log2_approx_cubic<bit_size>(f);
    return res;
}

template<size_t bit_size>
float __attribute__((noinline)) benchmark_approx_log_linear(const std::vector<float>& vec){
    Benchmark b;
    float res = 0.0f;
    for(auto f : vec) res += log2_approx_lerp<bit_size>(f);
    return res;
}

template<size_t bit_size>
std::array<float, 3> compare_result_linear(const std::vector<float>& vec){
    float min_err = 1000.0f;
    float max_err = 0.0f;
    float average = 0.0f;
    for(auto f : vec){
        float exact = std::log2(f);
        float approx = log2_approx_lerp<bit_size>(f);
        float diff = std::abs(exact - approx);
        min_err = std::min(min_err, diff);
        max_err = std::max(max_err, diff);
        average += diff;
    }
    average /= vec.size();
    return {min_err, max_err, average};
}

template<size_t bit_size>
std::array<float, 3> compare_result_cubic(const std::vector<float>& vec){
    float min_err = 1000.0f;
    float max_err = 0.0f;
    float average = 0.0f;
    for(auto f : vec){
        float exact = std::log2(f);
        float approx = log2_approx_cubic<bit_size>(f);
        float diff = std::abs(exact - approx);
        min_err = std::min(min_err, diff);
        max_err = std::max(max_err, diff);
        average += diff;
    }
    average /= vec.size();
    return {min_err, max_err, average};
}

template<size_t bit_size>
void perf_bench_linear(size_t N){
    auto vec = gen_random_vector(N);
    volatile float dummy;
    std::cout << "libmath time:\n";
    dummy = benchmark_libmath(vec);
    std::cout << "\n";
    std::cout << "linear approximation time:\n";
    dummy = benchmark_approx_log_linear<bit_size>(vec);
    std::cout << "\n";
}

template<size_t bit_size>
void perf_bench_cubic(size_t N){
    auto vec = gen_random_vector(N);
    volatile float dummy;
    std::cout << "libmath time:\n";
    dummy = benchmark_libmath(vec);
    std::cout << "\n";
    std::cout << "cubic approximation time:\n";
    dummy = benchmark_approx_log_cubic<bit_size>(vec);
    std::cout << "\n";
}

template<size_t bit_size>
void precision_bench_linear(float min, float max, size_t N){
    auto vec = gen_range_vector(min, max, N);
    auto [min_err, max_err, average] = compare_result_linear<bit_size>(vec);
    std::cout << "min error " << min_err << " ";
    std::cout << "max error " << max_err << " ";
    std::cout << "average error " << average << "\n";
}

template<size_t bit_size>
void precision_bench_cubic(float min, float max, size_t N){
    auto vec = gen_range_vector(min, max, N);
    auto [min_err, max_err, average] = compare_result_cubic<bit_size>(vec);
    std::cout << "min error " << min_err << " ";
    std::cout << "max error " << max_err << " ";
    std::cout << "average error " << average << "\n";
}

void example_precision_benchmark(){
    std::cout << "benchmarking precision of the cubic approximation of log2 using a 5 bit LUT\n";
    std::cout << "input range from 0.01 to 1000.0\n";
    std::cout << "using 100000 samples\n";
    precision_bench_cubic<5>(0.01f, 1000.0f, 100000ull);
    
    std::cout << "\n";
    
    
    std::cout << "benchmarking precision of the linear approximation of log2 using a 5 bit LUT\n";
    std::cout << "input range from 0.01 to 1000.0\n";
    std::cout << "using 100000 samples\n";
    precision_bench_linear<5>(0.01f, 1000.0f, 100000ull);
    
    std::cout << "\n\n";
}

void example_performance_benchmark(){
    std::cout << "benchmarking performance of the linear appximation of log2 using a 5 bit LUT\n";
    std::cout << "time it took to calculate log2 of 100000000 random floats:\n\n";
    perf_bench_linear<5>(100000000ull);
    
    std::cout << "\n";
    
    std::cout << "benchmarking performance of the cubic appximation of log2 using a 5 bit LUT\n";
    std::cout << "time it took to calculate log2 of 100000000 random floats:\n\n";
    perf_bench_cubic<5>(100000000ull);
}

int main(){
    
    //example usage:
    float f = 5.9262;
    std::cout << "log2 of " << f << " according to different algorithms:\n";
    std::cout << "libmath: " << std::log2(f) << "\n";
    std::cout << "linear approximation using 3 bit LUT: " << log2_approx_lerp<3>(f) << "\n";
    std::cout << "linear approximation using 5 bit LUT: " << log2_approx_lerp<5>(f) << "\n";
    std::cout << "cubic approximation using 3 bit LUT:  " << log2_approx_cubic<3>(f) << "\n";
    std::cout << "cubic approximation using 5 bit LUT:  " << log2_approx_cubic<5>(f) << "\n";
    std::cout << "\n";
    
    
    //some benchmarks:
    example_precision_benchmark();
    example_performance_benchmark();
}

