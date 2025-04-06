G4Allocator benchmarking
========================

Directory `plaincxx` contains a stand alone benchmark of G4Allocator, using STL Chronos library.

Directory `google_benchmark` contains a stand alone benchmark of G4Allocator, using Google benchmark library.

# Plain C++ test

Example of output:

```
./benchmark 
Benchmarking with 100000000 allocations:
Standard new/delete time: 1.24491 s
G4Allocator time: 0.73051 s
```

# Google benchmark test

Example of output

```
./test_bench 
2025-04-04T15:45:54+02:00
Running ./test_bench
Run on (8 X 4200 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 1280 KiB (x4)
  L3 Unified 8192 KiB (x1)
Load Average: 0.43, 0.40, 0.38
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
-------------------------------------------------------------------------
Benchmark                               Time             CPU   Iterations
-------------------------------------------------------------------------
BM_StandardNewDelete/100000       1287092 ns      1285104 ns          515
BM_StandardNewDelete/1000000     14206845 ns     14181593 ns           48
BM_StandardNewDelete/100000000 2641793093 ns   2638097318 ns            1
BM_G4Allocator/100000              353235 ns       352782 ns         1991
BM_G4Allocator/1000000            6118428 ns      6103214 ns           83
BM_G4Allocator/100000000       1257141718 ns   1254935547 ns            1

```

## How to install Google benchmark library


```
git clone https://github.com/google/benchmark.git
cmake -B build_benchmark -S benchmark -D CMAKE_INSTALL_PREFIX=/usr/local/benchmark  -DBENCHMARK_ENABLE_GTEST_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
sudo mkdir /usr/local/benchmark
sudo chown alvarotd:alvarotd -R /usr/local/benchmark
cmake --build build_benchmark -- -j 5  install
```

# Indirection test

This directory contains the results of indirection performance using G4Allocator, raw pointer or direct access. This is the results from the benchmark:

```
-------------------------------------------------------------------------------------
Benchmark                                           Time             CPU   Iterations
-------------------------------------------------------------------------------------
BM_G4AllocatorIndirection/iterations:10000      68162 ns        68063 ns        10000
BM_RawPointerIndirection/iterations:10000       69493 ns        69407 ns        10000
BM_DirectAccess/iterations:10000                59423 ns        59363 ns        10000
```
