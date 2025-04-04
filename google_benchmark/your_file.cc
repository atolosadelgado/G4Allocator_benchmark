#include <benchmark/benchmark.h>
#include <vector>
#include "G4Allocator.hh"
#include "G4Threading.hh"

///////////////////////
// Standard Object
class MyStandardObject {
public:
    int x, y, z;
    MyStandardObject() : x(0), y(0), z(0) {}
    virtual ~MyStandardObject() {}
};

///////////////////////
// G4 Allocated Object
class MyG4Object {
public:
    int x, y, z;
    MyG4Object() : x(0), y(0), z(0) {}
    virtual ~MyG4Object() {}

    void* operator new(size_t);
    void operator delete(void*);
};

G4ThreadLocal G4Allocator<MyG4Object>* myObjectAllocator = nullptr;

void* MyG4Object::operator new(size_t) {
    if (!myObjectAllocator) myObjectAllocator = new G4Allocator<MyG4Object>;
    return myObjectAllocator->MallocSingle();
}

void MyG4Object::operator delete(void* obj) {
    myObjectAllocator->FreeSingle((MyG4Object*)obj);
}

//////////////////////////
// Benchmark: Standard
static void BM_StandardNewDelete(benchmark::State& state) {
    const size_t numObjects = state.range(0);
    std::vector<MyStandardObject*> objects;
    objects.reserve(numObjects);

    for (auto _ : state) {
        for (size_t i = 0; i < numObjects; ++i)
            objects.push_back(new MyStandardObject());
        for (auto obj : objects)
            delete obj;
        objects.clear();
    }
}

//////////////////////////
// Benchmark: G4Allocator
static void BM_G4Allocator(benchmark::State& state) {
    const size_t numObjects = state.range(0);
    std::vector<MyG4Object*> objects;
    objects.reserve(numObjects);

    for (auto _ : state) {
        for (size_t i = 0; i < numObjects; ++i)
            objects.push_back(new MyG4Object());
        for (auto obj : objects)
            delete obj;
        objects.clear();
    }
}

// Register benchmarks
BENCHMARK(BM_StandardNewDelete)->Arg(100000)->Arg(1000000)->Arg(100000000);
BENCHMARK(BM_G4Allocator)->Arg(100000)->Arg(1000000)->Arg(100000000);

BENCHMARK_MAIN();

