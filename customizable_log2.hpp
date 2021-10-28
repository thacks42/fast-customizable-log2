#pragma once
#include <array>
#include <cstdint>
#include <bit>
#include "log2_constexpr.hpp"


template<size_t N>
constexpr std::array<float, N+1> gen_log2_lerp_lookup(){
    std::array<float, N+1> result;
    constexpr float m = 1.0f / static_cast<float>(N);
    for(size_t i = 0; i < N+1; i++){
        float x = 1.0f + m * static_cast<float>(i);
        result[i] = static_cast<float>(approx_log2(static_cast<double>(x)));
    }
    return result;
}



template<size_t N>
constexpr std::array<float, N+3> gen_log2_cubic_lookup(){
    std::array<float, N+3> result;
    constexpr float m = 1.0f / static_cast<float>(N);
    for(size_t i = 0; i < N+3; i++){
        result[i] = std::log2(1.0f + m * (static_cast<float>(i) - 1.0f));
    }
    return result;
}

template<size_t N>
constexpr std::array<float, N+3> gen_log2_cubic_lookup_2(){
    std::array<float, N+3> result;
    constexpr float m = 1.0f / static_cast<float>(N);
    for(size_t i = 0; i < N+3; i++){
        result[i] = static_cast<float>(approx_log2(static_cast<double>(1.0f + m * (static_cast<float>(i) - 1.0f))));
    }
    return result;
}

template<size_t LUT_bit_size>
float log2_approx_lerp(float f){
    uint32_t x = std::bit_cast<uint32_t>(f);
    int32_t exponent = ((x >> 23)&0xff) - 0x7f;
    uint32_t mantissa = x & 0x7fffff;
    
    constexpr int32_t table_size = (1 << LUT_bit_size);
    constexpr static auto LUT = gen_log2_lerp_lookup<table_size>();
    
    constexpr int32_t mask1 = table_size-1;
    constexpr int32_t mask2 = (1<<(23-LUT_bit_size)) - 1;
    
    int32_t index0 = (mantissa >> (23-LUT_bit_size)) & mask1;
    int32_t index1 = index0 + 1;
    
    uint32_t mix_i = 0x3f800000 | ((mantissa & mask2) << LUT_bit_size);
    float mix = std::bit_cast<float>(mix_i);
    mix -= 1.0f;
    float result = static_cast<float>(exponent) + LUT[index0] * (1.0f - mix) + LUT[index1] * mix;
    return result;
}

template<size_t LUT_bit_size>
float log2_approx_cubic(float f){
    uint32_t x = std::bit_cast<uint32_t>(f);
    int32_t exponent = ((x >> 23)&0xff) - 0x7f;
    uint32_t mantissa = x & 0x7fffff;
    
    constexpr int32_t table_size = (1 << LUT_bit_size);
    constexpr static auto LUT = gen_log2_cubic_lookup_2<table_size>();
    
    
    constexpr int32_t mask1 = table_size-1;
    constexpr int32_t mask2 = (1<<(23-LUT_bit_size)) - 1;
    
    int32_t index0 = ((mantissa >> (23-LUT_bit_size)) & mask1);
    int32_t index1 = index0 + 1;
    int32_t index2 = index0 + 2;
    int32_t index3 = index0 + 3;
    
    float p0 = LUT[index0];
    float p1 = LUT[index1];
    float p2 = LUT[index2];
    float p3 = LUT[index3];

    uint32_t t_i = 0x3f800000 | ((mantissa & mask2) << LUT_bit_size);
    float t = std::bit_cast<float>(t_i);
    t -= 1.0f;
    
    float tt = t*t;
    float ttt = t*tt;
    
    float m0 = 0.5f * ( (p2-p1) + (p1-p0) );
    float m1 = 0.5f * ( (p3-p2) + (p2-p1) );
    
    float h0 = 2.0f * ttt - 3.0f * tt + 1.0f;
    float h1 = ttt - 2.0f * tt  + t;
    float h2 = -2.0f * ttt + 3.0f * tt;
    float h3 = ttt - tt;
    
    float result = static_cast<float>(exponent) + h0 * p1 + h1 * m0 + h2 * p2 + h3 * m1;
    
    return result;
}
