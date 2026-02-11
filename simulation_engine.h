#pragma once
#include "board.h"
#include "resource_manager.h"
#include <omp.h>
#include <QOpenGLFunctions_4_5_Core>

class SimulationEngine : protected QOpenGLFunctions_4_5_Core
{
public:
    SimulationEngine(Board *board);
    ~SimulationEngine();
    void Restart(Board* board);
    inline void Increment(){counter++;}
    void InitSsbos(Board *board);
    void InitNetworkData(uint32_t* species);
    void Process(uint32_t id_size, uint32_t* ids, float* inputData, float* outputData);
    void killNetwork(int id);
    void reproduceNetwork(int parentA, int parentB, int childIdx);
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
    GLuint ssboEmpty;
    GLsync fences[2] = {0,0};
    float* networksPtr = nullptr;
    float* stagingPtr = nullptr;
    uint32_t* idPtr = nullptr;
    uint32_t* emptyPtr = nullptr;
    const size_t BATCH_SIZE = 1000;
    Shader *shader;
    size_t bWidth,bHeight;
    size_t bCapacity;
    size_t bSize;
    int counter = 0;
};
