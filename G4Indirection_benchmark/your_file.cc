#include <benchmark/benchmark.h>
#include <vector>
#include <iostream>
#include "G4Allocator.hh"
#include "G4Threading.hh"  // For G4ThreadLocal

// Example objects using G4Allocator
class Inner {
public:
    std::vector<int> values;
    Inner();  // Constructor
    virtual ~Inner() {}

    void* operator new(size_t);  // Declare operator new
    void operator delete(void*); // Declare operator delete
};

class Middle {
public:
    Inner* inner;
    Middle();  // Constructor
    virtual ~Middle() {}

    void* operator new(size_t);  // Declare operator new
    void operator delete(void*); // Declare operator delete
};

class Outer {
public:
    Middle* middle;
    Outer();  // Constructor
    virtual ~Outer() {}

    void* operator new(size_t);  // Declare operator new
    void operator delete(void*); // Declare operator delete
};

// G4ThreadLocal allocator for thread safety
G4ThreadLocal G4Allocator<Inner>* innerAllocator = nullptr;
G4ThreadLocal G4Allocator<Middle>* middleAllocator = nullptr;
G4ThreadLocal G4Allocator<Outer>* outerAllocator = nullptr;

// Constructor definitions
Inner::Inner() {
    values.resize(10000);  // Initialize the vector with size
}

Middle::Middle() {
    inner = nullptr;  // Set the inner pointer to nullptr
}

Outer::Outer() {
    middle = nullptr;  // Set the middle pointer to nullptr
}

// G4Allocator implementation for MyObject
void* Inner::operator new(size_t) {
    if (!innerAllocator) innerAllocator = new G4Allocator<Inner>;
    return innerAllocator->MallocSingle();  // Allocate using G4Allocator
}

void Inner::operator delete(void* obj) {
    innerAllocator->FreeSingle((Inner*)obj);  // Free using G4Allocator
}

void* Middle::operator new(size_t) {
    if (!middleAllocator) middleAllocator = new G4Allocator<Middle>;
    return middleAllocator->MallocSingle();  // Allocate using G4Allocator
}

void Middle::operator delete(void* obj) {
    middleAllocator->FreeSingle((Middle*)obj);  // Free using G4Allocator
}

void* Outer::operator new(size_t) {
    if (!outerAllocator) outerAllocator = new G4Allocator<Outer>;
    return outerAllocator->MallocSingle();  // Allocate using G4Allocator
}

void Outer::operator delete(void* obj) {
    outerAllocator->FreeSingle((Outer*)obj);  // Free using G4Allocator
}

// Benchmark using G4Allocator for Indirection
static void BM_G4AllocatorIndirection(benchmark::State& state) {
    const size_t numElements = 10000;

    // Allocate memory for Outer, Middle, and Inner objects from memory pool (G4Allocator)
    std::vector<Outer*> outers(numElements);
    std::vector<Middle*> middles(numElements);
    std::vector<Inner*> inners(numElements);

    // Initialize the objects
    for (size_t i = 0; i < numElements; ++i) {
        inners[i] = new Inner();   // Use G4Allocator to allocate Inner
        inners[i]->values[i] = i;
        middles[i] = new Middle(); // Use G4Allocator to allocate Middle
        middles[i]->inner = inners[i];
        outers[i] = new Outer();   // Use G4Allocator to allocate Outer
        outers[i]->middle = middles[i];
    }

    int sum = 0;
    for (auto _ : state) {
        for (size_t i = 0; i < numElements; ++i) {
            sum += outers[i]->middle->inner->values[i];  // Indirect access
        }
    }

    benchmark::DoNotOptimize(sum);

    // Cleanup (delete allocated objects)
    for (size_t i = 0; i < numElements; ++i) {
        delete outers[i]; // G4Allocator delete for Outer
    }
}

// Benchmark using raw pointer indirection for comparison
static void BM_RawPointerIndirection(benchmark::State& state) {
    const size_t numElements = 10000;

    // Allocate memory for Outer, Middle, and Inner objects using raw pointers
    std::vector<Outer*> outers(numElements);
    std::vector<Middle*> middles(numElements);
    std::vector<Inner*> inners(numElements);

    // Initialize the objects
    for (size_t i = 0; i < numElements; ++i) {
        inners[i] = new Inner();   // Raw pointer allocation
        inners[i]->values[i] = i;
        middles[i] = new Middle(); // Raw pointer allocation
        middles[i]->inner = inners[i];
        outers[i] = new Outer();   // Raw pointer allocation
        outers[i]->middle = middles[i];
    }

    int sum = 0;
    for (auto _ : state) {
        for (size_t i = 0; i < numElements; ++i) {
            sum += outers[i]->middle->inner->values[i];  // Indirect access
        }
    }

    benchmark::DoNotOptimize(sum);

    // Cleanup (delete allocated objects)
    for (size_t i = 0; i < numElements; ++i) {
        delete outers[i]; // Raw pointer delete for Outer
    }
}

// Benchmark using direct access for comparison
static void BM_DirectAccess(benchmark::State& state) {
    const size_t numElements = 10000;

    // Use a single contiguous vector to store data, no indirection
    std::vector<Inner> inners(numElements);

    // Initialize the objects
    for (size_t i = 0; i < numElements; ++i) {
        inners[i].values[i] = i;
    }

    int sum = 0;
    for (auto _ : state) {
        for (size_t i = 0; i < numElements; ++i) {
            sum += inners[i].values[i];  // Direct access
        }
    }

    benchmark::DoNotOptimize(sum);
}

BENCHMARK(BM_G4AllocatorIndirection)->Iterations(10000);
BENCHMARK(BM_RawPointerIndirection)->Iterations(10000);
BENCHMARK(BM_DirectAccess)->Iterations(10000);

BENCHMARK_MAIN();

