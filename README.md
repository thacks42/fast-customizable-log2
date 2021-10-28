# fast-customizable-log2

## Quick start:

compile using:

`clang++ -std=c++20 -g -O3 -march=native -Wall -Wpedantic -ffast-math main.cpp`

This contains a few performance and acccuracy benchmarks. Enjoy.

## How it works

This code implements a reasonably fast log2 approximation with customizable precision/performance.

It provides two templated functions: `log2_approx_lerp<N>` and `log2_approx_cubic<N>` which perform a linear/cubic approximation of log2 using "fancy math" and a (small) lookup table (LUT).
The template parameter can be used to control the size of the LUT as a power of two: LUT-size = 2^N.

Note that none of the implementations handle `NaN`s or `inf`. You could implement proper `NaN` and `inf` handling, but those are outside the scope of this project.

## Performance

It appears that gcc does not properly optimize the code, which is why I'd recommend using clang if you want to get the best possible performance.

You can find the generated assembly here: <https://godbolt.org/z/xx35M31fo>

Using the provided flags, these are the results I get on my hardware:

```
benchmarking precision of the cubic approximation of log2 using a 5 bit LUT
input range from 0.01 to 1000.0
using 100000 samples
min error 0 max error 1.90735e-06 average error 4.05972e-07

benchmarking precision of the linear approximation of log2 using a 5 bit LUT
input range from 0.01 to 1000.0
using 100000 samples
min error 0 max error 0.000171185 average error 5.93866e-05


benchmarking performance of the linear appximation of log2 using a 5 bit LUT
time it took to calculate log2 of 100000000 random floats:

libmath time:
396 ms

linear approximation time:
65 ms


benchmarking performance of the cubic appximation of log2 using a 5 bit LUT
time it took to calculate log2 of 100000000 random floats:

libmath time:
403 ms

cubic approximation time:
135 ms
```
