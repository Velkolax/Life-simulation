#pragma once
#include "board.h"
#include "resource_manager.h"
#include <omp.h>

// 100000 mieści się w 4GB VRAM
// 30000 jest w miarę ok
constexpr size_t NUMBER_OF_BACTERIA = 400; //temp
constexpr size_t NUMBER_OF_ACTIVE_BACTERIA = 300; //temp
constexpr size_t STRIDE = NUMBER_OF_BACTERIA;


class SimulationEngine
{
public:
    SimulationEngine(Board *board);
    ~SimulationEngine();
    void InitSsbos(Board *board);
    void InitNetworkData();
    void ssbo_barrier();
    void SendData(float *f);
    void Tick(int id_size, int *ids, float* inputData, float* outputData);
    void killNetwork(int id);
    void reproduceNetwork(int idA,int idB);
    inline size_t getbCapacity(){return bCapacity;}
    inline size_t getbSize(){return bSize;}
    float* InPtr = nullptr;
    float* OutPtr = nullptr;
    uint64_t tickCounter = 0;
private:


    GLuint ssboNetworks;
    GLuint ssboStaging;
    GLuint ssboIn;
    GLuint ssboOut;
    GLuint ssboIds;
    GLsync fences[2] = {0,0};
    float* networksPtr = nullptr;
    float* stagingPtr = nullptr;
    std::vector<DataInOut> ramBuffer;
    const size_t BATCH_SIZE = 1000;
    Shader shader;
    size_t bWidth,bHeight;
    size_t bCapacity;
    size_t bSize;
};
