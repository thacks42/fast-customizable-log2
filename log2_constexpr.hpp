#pragma once

#include <cstring>
#include <cmath>
#include <cstdint>
#include <utility>
#include <bit>

constexpr std::pair<double, double> get_exponent_and_normalize(double d){
    uint64_t x = std::bit_cast<uint64_t>(d);
    uint64_t e = x >> 52;
    uint64_t m = x & 0xfffffffffffffull;
    uint64_t new_exp = 0x3ff0000000000000ull;
    uint64_t normalized = new_exp | m;
    double new_d = std::bit_cast<double>(normalized);
    int64_t res = e & 0x7ff;
    res -= 0x3ff;
    return {static_cast<double>(res), new_d};
}


constexpr std::pair<float, float> get_exponent_and_normalize(float f){
    uint32_t x = std::bit_cast<uint32_t>(f);
    uint32_t e = x >> 23;                     //exponent of f
    uint32_t m = x & 0x7fffff;                //mantissa of f
    uint32_t new_exp = 0x3f800000;            //exponent = 0 -> float is in the range [1, 2)
    uint32_t normalized = new_exp | m;        //this is now the binary representation of a normalized 'f'
    float new_f = std::bit_cast<float>(normalized);
    //std::memcpy(&new_f, &normalized, sizeof(float));  //copy back to float
    int32_t res = e & 0xff;                   //we remove the sign bit (should be 0 anyway)
    res -= 0x7f;                              //compute the actual exponent (ieee floats have a bias of 127 i.e. 0x7f)
    return {static_cast<float>(res), new_f};           //return the exponent as a float. note that this is not a bit-reinterpretation but an actual conversion
}

constexpr double pow_neg_two(uint64_t m){
    m = 0x3ff - m;
    m = m << 52;
    double d = std::bit_cast<double>(m);
    return d;
}

constexpr float pow_neg_two(uint32_t m){                //calculates 2^(-m)
    m = 127 - m;
    m = m << 23;
    float f = std::bit_cast<float>(m);
    return f;
}

constexpr uint64_t log2_step(double& d){
    uint64_t m = 0;
    while(d < 2.0){
        d = d * d;
        m++;
    }
    d /= 2.0;
    return m;
}

constexpr uint32_t log2_step(float& f){
    uint32_t m = 0;
    while(f < 2.0f){                          //square f until it is in the (2, 4] range
        f = f*f;
        m++;
    }
    f /= 2.0f;                                //re-normalize in the (1, 2] range
    return m;                                 //return the number of squarings we did
}


constexpr double approx_log2(double d){
    auto [e, new_d] = get_exponent_and_normalize(d);
    d = new_d;
    if(d == 1.0) return e;
    double res = e;
    uint64_t m = 0;
    for(int i = 0; i < 30; i++){
        m += log2_step(d);
        res += pow_neg_two(m);
        if(d == 1.0) return res;
    }
    return res;
}

constexpr float approx_log2(float f){
    auto [e, new_f] = get_exponent_and_normalize(f); //step 1: normalization
    f = new_f;
    if(f == 1.0f) return e;                  //f = 2^n where `n` is an integer -> we're done
    float res = e;                           //this will be our result. Initialize it to the exponent
    uint32_t m = 0;
    for(int i = 0; i < 30; i++){              //fixed number of iterations. See description at the top for more information
        m += log2_step(f);
        res += pow_neg_two(m);
        if(f == 1.0f) return res;            //no more digits to find -> terminate
    }
    return res;
}

