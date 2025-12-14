#pragma once
#include "board.h"
#include "resource_manager.h"

struct Counters
{
    uint32_t aliveCount;
    uint32_t stackTop;
    uint32_t padding[2];
};



class SimulationEngine
{
public:
    SimulationEngine(Board *board);
    ~SimulationEngine();
    void InitSsbos(Board *board);
    GLuint GetBacteriaSSBO() const { return ssboBacteria; }
    GLuint GetGridSSBO() const { return ssboGrid; }
    void ssbo_barrier();
    void Tick();
    void MoveBacteria(Shader& shader);
private:


    GLuint ssboBacteria;
    GLuint ssboGrid;
    GLuint ssboFreeList;
    GLuint ssboCounters;
    int bWidth,bHeight;
};
