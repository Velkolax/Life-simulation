//
// Created by tk2 on 12/14/25.
//

#include "simulation_engine.h"

#include "BacteriaData.h"
#include "GLFW/glfw3.h"

SimulationEngine::SimulationEngine(Board* board)
{
    InitSsbos(board);
    bWidth = board->getWidth();
    bHeight = board->getHeight();
}

void SimulationEngine::InitSsbos(Board *board)
{
    int boardSize = board->getHeight()*board->getWidth();
    std::vector<BacteriaData> b; b.reserve(boardSize);
    std::vector<uint32_t> freePlaces(boardSize,0);
    std::vector<int32_t> boardData(boardSize, 0);


    int id = 0;
    for (int i=0;i<boardSize;i++)
    {
        Hexagon *h = board->getHexagon(i);
        if (bacteria(h->getResident()))
        {
            b.emplace_back(h->getPos(),id,1,1000);
            boardData[i]=id+1;
            id++;
        }
        if (wall(h->getResident())) boardData[i]=-1;
    }
    for (int i=id;i<boardSize;i++)
    {
        b.emplace_back(glm::ivec2(-2137,-2137),id,0,0);
        freePlaces.push_back(i);
    }
    Counters counters;
    counters.aliveCount = id;
    counters.stackTop = boardSize-id;

    auto createSSBO = [](GLuint& id, int binding, const void* data, size_t size) {
        glGenBuffers(1, &id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    };
    // for (auto &bac : b)
    // {
    //     if (bac.alive==1)
    //     {
    //         std::cout << "------------------- NETWORK ------------------" << std::endl;
    //         for (int i=0;i<NETWORK_SIZE_VEC4;i++)
    //         {
    //             std::cout << bac.network[i].x << " " << bac.network[i].y << " " << bac.network[i].z << " " << bac.network[i].w << std::endl;
    //         }
    //     }
    //
    // }

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

void SimulationEngine::MoveBacteria()
{

    Shader &shader = ResourceManager::GetShader("movement");
    shader.Use();
    shader.SetInteger("bWidth",bWidth);
    shader.SetInteger("bHeight",bHeight);
    shader.SetFloat("time",(float)glfwGetTime());
    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboBacteria);
    // glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboGrid);
    if (glDispatchCompute == NULL) {
        std::cout << "CRITICAL ERROR: glDispatchCompute is NULL! OpenGL functions not loaded correctly." << std::endl;
        exit(-1);
    }
    int bCount = bWidth*bHeight;
    int groupSize = 256;

    glDispatchCompute((bCount+groupSize-1)/groupSize,1,1);
    ssbo_barrier();
}


void SimulationEngine::PassTime()
{
    Shader &shader = ResourceManager::GetShader("passTime");
    shader.Use();
    shader.SetInteger("dt",1);
    int bCount = bWidth*bHeight;
    int groupSize = 256;

    glDispatchCompute((bCount+groupSize-1)/groupSize,1,1);
    ssbo_barrier();
}

void SimulationEngine::Tick()
{
    MoveBacteria();
    PassTime();
}
