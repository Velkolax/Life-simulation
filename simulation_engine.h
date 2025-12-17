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
// 30000 jest w miarę ok
constexpr size_t NUMBER_OF_BACTERIA = 400; //temp
constexpr size_t NUMBER_OF_ACTIVE_BACTERIA = 300; //temp
constexpr size_t STRIDE = NUMBER_OF_BACTERIA;

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
    void Tick(std::function<void(DataInOut*)> updateCallback = nullptr);
    void MoveBacteria();
    void PassTime();
    DataInOut* InOutsPtr[2] = {nullptr,nullptr};
    uint64_t tickCounter = 0;
private:


    GLuint ssboNetworks;
    GLuint ssboStaging;
    GLuint ssboInOuts[2];
    GLsync fences[2] = {0,0};
    float* networksPtr = nullptr;
    float* stagingPtr = nullptr;

    const size_t BATCH_SIZE = 1000;
    Shader shader;
    int bWidth,bHeight;
};
