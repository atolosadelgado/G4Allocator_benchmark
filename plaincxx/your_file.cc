#include <iostream>
#include <chrono>
#include "G4Allocator.hh"
#include "G4Threading.hh"  // For G4ThreadLocal

class MyObject {
public:
    int x, y, z;
    MyObject() : x(0), y(0), z(0) {}
    virtual ~MyObject() {}

    void* operator new(size_t);
    void operator delete(void*);
};

G4ThreadLocal G4Allocator<MyObject>* myObjectAllocator = nullptr;

void* MyObject::operator new(size_t) {
    if (!myObjectAllocator) myObjectAllocator = new G4Allocator<MyObject>;
    return myObjectAllocator->MallocSingle();
}

void MyObject::operator delete(void* obj) {
    myObjectAllocator->FreeSingle((MyObject*)obj);
}

// Benchmark using G4Allocator
void benchmarkG4Allocator(size_t numObjects) {
    std::vector<MyObject*> objects;
    objects.reserve(numObjects);

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < numObjects; ++i) {
        objects.push_back(new MyObject());
    }

    for (auto obj : objects) {
        delete obj;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << "G4Allocator time: " << diff.count() << " s\n";
}

// Benchmark using standard new/delete
void benchmarkStandardAllocator(size_t numObjects) {
    std::vector<MyObject*> objects;
    objects.reserve(numObjects);

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < numObjects; ++i) {
        objects.push_back(new MyObject());
    }

    for (auto obj : objects) {
        delete obj;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << "Standard new/delete time: " << diff.count() << " s\n";
}

int main() {
    const size_t N = 100000000;

    std::cout << "Benchmarking with " << N << " allocations:\n";
    benchmarkStandardAllocator(N);
    benchmarkG4Allocator(N);

    return 0;
}

