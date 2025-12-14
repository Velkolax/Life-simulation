//
// Created by tk2 on 12/14/25.
//

#include "simulation_engine.h"

#include "BacteriaData.h"

SimulationEngine::SimulationEngine(Board* board)
{
    InitSsbos(board);
    bWidth = board->getWidth();
    bHeight = board->getHeight();
}

void SimulationEngine::InitSsbos(Board *board)
{
    int boardSize = board->getHeight()*board->getWidth();
    std::vector<BacteriaData> b;
    std::vector<uint32_t> freePlaces(boardSize,0);
    std::vector<int32_t> boardData(boardSize, 0);


    int id = 0;
    for (int i=0;i<boardSize;i++)
    {
        Hexagon *h = board->getHexagon(i);
        if (bacteria(h->getResident()))
        {
            b.emplace_back(h->getPos(),id,1,100);
            boardData[i]=id+1;
            id++;
        }
    }
    for (int i=id;i<boardSize;i++)
    {
        b.emplace_back(glm::ivec2(-2137,-2137),id,0,0);
        freePlaces.push_back(i);
    }
    Counters counters;
    counters.aliveCount = id;
    counters.stackTop = boardSize-id;

    for (int i=0;i<boardSize;i++)
    {
        Hexagon *h = board->getHexagon(i);
        if (wall(h->getResident())) boardData[i]=-1;
    }

    auto createSSBO = [](GLuint& id, int binding, const void* data, size_t size) {
        glGenBuffers(1, &id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    };

    for (int i=0;i<boardData.size();i++) std::cout << boardData[i] << std::endl;
    createSSBO(ssboGrid,      1, boardData.data(), boardData.size() * sizeof(int32_t));
    createSSBO(ssboBacteria,    0, b.data(),    b.size() * sizeof(BacteriaData));

    createSSBO(ssboFreeList,  2, freePlaces.data(),  freePlaces.size() * sizeof(uint32_t));
    createSSBO(ssboCounters,  3, &counters,        sizeof(Counters));

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SimulationEngine::ssbo_barrier()
{
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
