//
// Created by tk2 on 12/14/25.
//

#include "simulation_engine.h"
#include <glm/gtc/type_precision.hpp>
#include "BacteriaData.h"
#include "GLFW/glfw3.h"

SimulationEngine::SimulationEngine(Board* board)
{
    InitSsbos(board);
    InitNetworkData();
    bWidth = board->getWidth();
    bHeight = board->getHeight();
    shader = ResourceManager::GetShader("network");
}



void SimulationEngine::InitSsbos(Board *board)
{

    /* CREATING NETWORK BUFFER */

    size_t ssboNetworksSize = sizeof(float) * SIZE * NUMBER_OF_BACTERIA;
    if (ssboNetworksSize > 4000000000) std::cout << "UPEWNIJ SIĘ ŻE TWÓJ VRAM JEST WIĘKSZY NIŻ 4GB" << std::endl;
    std::cout << ssboNetworksSize << std::endl;
    glCreateBuffers(1,&ssboNetworks);
    glNamedBufferStorage(ssboNetworks,ssboNetworksSize,nullptr,GL_DYNAMIC_STORAGE_BIT);

    GLbitfield stagingFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    size_t stagingSize = sizeof(float) * BATCH_SIZE * SIZE;
    glCreateBuffers(1,&ssboStaging);
    glNamedBufferStorage(ssboStaging,stagingSize,nullptr,stagingFlags);
    stagingPtr = (float*)glMapNamedBufferRange(ssboStaging,0,stagingSize,stagingFlags);

    /* CREATING INOUT BUFFERS */

    GLbitfield InOutFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    GLsizeiptr ssboInOutsSize = (GLsizeiptr)(NUMBER_OF_BACTERIA * INOUT_SIZE);
    glCreateBuffers(2,ssboInOuts);
    for (int i=0;i<2;i++)
    {
        glNamedBufferStorage(ssboInOuts[i],ssboInOutsSize,nullptr,InOutFlags);
        InOutsPtr[i] = (DataInOut*)glMapNamedBufferRange(ssboInOuts[i],0,ssboInOutsSize,InOutFlags);
        memset(InOutsPtr[i],0,ssboInOutsSize);
    }
}

void SimulationEngine::InitNetworkData()
{
    /* COPYING DATA TO VRAM WITH BATCHES */

    for (size_t offset = 0;offset < NUMBER_OF_BACTERIA;offset+=BATCH_SIZE)
    {
        size_t localBacteriaCount = std::min(BATCH_SIZE,NUMBER_OF_BACTERIA-offset);
        #pragma omp parallel for
        for (int i=0;i<localBacteriaCount;i++)
        {
            float* netDest = stagingPtr + (i * SIZE);
            int layers[] = {INPUT,HIDDEN1,HIDDEN2,HIDDEN3,OUTPUT};
            NeuralNetwork nn = buildNetwork(5,layers);
            memcpy(netDest,nn.neurons,(nn.neuronCount+nn.connectionCount)*sizeof(float));
            freeNetwork(&nn);
        }

        glCopyNamedBufferSubData(ssboStaging,ssboNetworks,0,offset*SIZE*sizeof(float),localBacteriaCount*SIZE*sizeof(float));
        GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
        GLenum result = glClientWaitSync(fence,GL_SYNC_FLUSH_COMMANDS_BIT,10000000000);
        glDeleteSync(fence);
    }
    glUnmapNamedBuffer(ssboStaging);
    glDeleteBuffers(1,&ssboStaging);
    stagingPtr = nullptr;
}


void SimulationEngine::Tick(std::function<void(DataInOut*)> updateCallback)
{
    GLsizeiptr ssboInOutsSize = (GLsizeiptr)(NUMBER_OF_BACTERIA * INOUT_SIZE);
    int readIdx = tickCounter % 2;
    int writeIdx = (tickCounter+1) % 2;


    if (fences[readIdx])
    {
        GLenum waitReturn = glClientWaitSync(fences[readIdx], GL_SYNC_FLUSH_COMMANDS_BIT, 10000000);
        glDeleteSync(fences[readIdx]);
        fences[readIdx]=0;
    }

    if (updateCallback != nullptr)
    {
        updateCallback(InOutsPtr[readIdx]);
    }
    glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
    shader.Use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,ssboNetworks);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,ssboInOuts[readIdx]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,ssboInOuts[writeIdx]);
    shader.SetInteger("activeBacteria",NUMBER_OF_ACTIVE_BACTERIA);
    glDispatchCompute((NUMBER_OF_ACTIVE_BACTERIA + 63) / 64, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    if (fences[writeIdx]) glDeleteSync(fences[writeIdx]);
    fences[writeIdx] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE,0);
    tickCounter++;
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "OpenGL Error: " << err << std::endl;
    }
}
