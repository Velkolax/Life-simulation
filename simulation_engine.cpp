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
    size_t globalStride = NUMBER_OF_BACTERIA;

    for (size_t offset = 0; offset < NUMBER_OF_BACTERIA; offset += BATCH_SIZE)
    {
        size_t localBacteriaCount = std::min(BATCH_SIZE, NUMBER_OF_BACTERIA - offset);
        std::vector<float> tempBuffer(localBacteriaCount * SIZE);

        #pragma omp parallel for
        for (int i = 0; i < localBacteriaCount; i++)
        {
            int layers[] = {INPUT, HIDDEN1, HIDDEN2, HIDDEN3, OUTPUT};
            NeuralNetwork nn = buildNetwork(5, layers);
            initializeRandom(&nn);
            int paramIdx = 0;
            for(int j=0; j<nn.neuronCount; j++) {
                tempBuffer[j * localBacteriaCount + i] = nn.neurons[j];
                paramIdx++;
            }
            for(int j=0; j<nn.connectionCount; j++) {
                tempBuffer[(paramIdx + j) * localBacteriaCount + i] = nn.connections[j];
            }
            freeNetwork(&nn);
        }
        memcpy(stagingPtr, tempBuffer.data(), tempBuffer.size() * sizeof(float));

        for (int p = 0; p < SIZE; p++)
        {
            size_t srcOffset = p * localBacteriaCount * sizeof(float);
            size_t dstOffset = (p * globalStride + offset) * sizeof(float);

            glCopyNamedBufferSubData(
                ssboStaging,
                ssboNetworks,
                srcOffset,
                dstOffset,
                localBacteriaCount * sizeof(float)
            );
        }
        GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 10000000000);
        glDeleteSync(fence);
    }

    glUnmapNamedBuffer(ssboStaging);
    glDeleteBuffers(1, &ssboStaging);
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

    static std::vector<DataInOut> ramBuffer;
    if (ramBuffer.size() < NUMBER_OF_BACTERIA) {
        ramBuffer.resize(NUMBER_OF_BACTERIA);
    }

    size_t dataSize = NUMBER_OF_ACTIVE_BACTERIA * sizeof(DataInOut);
    memcpy(ramBuffer.data(), InOutsPtr[readIdx], dataSize);
    //std::cout << ramBuffer[0].output[0] << std::endl;
    memcpy(InOutsPtr[readIdx], ramBuffer.data(), dataSize);




    glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
    shader.Use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,ssboNetworks);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,ssboInOuts[readIdx]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,ssboInOuts[writeIdx]);
    shader.SetInteger("activeBacteria",NUMBER_OF_ACTIVE_BACTERIA);
    shader.SetInteger("stride",STRIDE);
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
