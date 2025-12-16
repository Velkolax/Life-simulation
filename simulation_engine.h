#pragma once
#include "board.h"
#include "resource_manager.h"
#include <omp.h>

struct Counters
{
    uint32_t aliveCount;
    uint32_t stackTop;
    uint32_t padding[2];
};

// ALL NETWORKS SHOULD HAVE THIS SIZE
constexpr size_t INPUT = 64;
constexpr size_t HIDDEN1 = 80;
constexpr size_t HIDDEN2 = 32;
constexpr size_t HIDDEN3 = 16;
constexpr size_t OUTPUT = 8;
constexpr size_t SIZE = INPUT * HIDDEN1 + HIDDEN1 + HIDDEN1 * HIDDEN2 + HIDDEN2 + HIDDEN2 * HIDDEN3 + HIDDEN3 + HIDDEN3 * OUTPUT + OUTPUT;

// 100000 mieści się w 4GB VRAM
constexpr size_t NUMBER_OF_BACTERIA = 100000; //temp

struct alignas(16) DataInOut {
    float input[INPUT];
    float output[OUTPUT];
};

constexpr size_t INOUT_SIZE = sizeof(DataInOut);


class SimulationEngine
{
public:
    SimulationEngine(Board *board);
    ~SimulationEngine();
    void InitSsbos(Board *board);
    void InitNetworkData();
    void ssbo_barrier();
    void Tick();
    void MoveBacteria();
    void PassTime();
private:


    GLuint ssboNetworks;
    GLuint ssboStaging;
    GLuint ssboInOuts[2];
    float* networksPtr = nullptr;
    float* stagingPtr = nullptr;
    uint64_t tickCounter = 0;
    const size_t BATCH_SIZE = 1000;
    DataInOut* InOutsPtr[2] = {nullptr,nullptr};
    int bWidth,bHeight;
};
