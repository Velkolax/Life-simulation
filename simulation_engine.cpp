#include "simulation_engine.h"
#include <glm/gtc/type_precision.hpp>
#include "BacteriaData.h"
#include "game_configdata.h"
#include "GLFW/glfw3.h"

SimulationEngine::SimulationEngine(Board* board)
{
    bWidth = board->getWidth();
    bHeight = board->getHeight();
    bCapacity = bWidth * bHeight;
    bSize = board->getBacteriaCount();
    shader = ResourceManager::GetShader("network");
    std::cout << "BCAPACITY: " << bCapacity << " BSIZE: " << bSize << std::endl;
    InitSsbos(board);
    InitNetworkData();
}

SimulationEngine::~SimulationEngine()
{
    if (stagingPtr) glUnmapNamedBuffer(ssboStaging);
    if (InPtr) glUnmapNamedBuffer(ssboIn);
    if (OutPtr) glUnmapNamedBuffer(ssboOut);
    if (idPtr) glUnmapNamedBuffer(ssboIds);

    glDeleteBuffers(1,&ssboNetworks);
    glDeleteBuffers(1,&ssboStaging);
    glDeleteBuffers(1,&ssboIn);
    glDeleteBuffers(1,&ssboOut);
    glDeleteBuffers(1,&ssboIds);
}

void SimulationEngine::Restart()
{
    InitNetworkData();
}


void SimulationEngine::InitSsbos(Board *board)
{

    /* CREATING NETWORK BUFFER */

    size_t ssboNetworksSize = sizeof(float) * SIZE * bCapacity;
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
    GLsizeiptr ssboInSize = (GLsizeiptr)(bCapacity * INPUT * sizeof(float));
    GLsizeiptr ssboOutSize = (GLsizeiptr)(bCapacity * OUTPUT * sizeof(float));


    glCreateBuffers(1,&ssboIn);
    glNamedBufferStorage(ssboIn,ssboInSize,nullptr,InOutFlags);
    InPtr = (float*)glMapNamedBufferRange(ssboIn,0,ssboInSize,InOutFlags);
    memset(InPtr,0,ssboInSize);


    glCreateBuffers(1,&ssboOut);
    glNamedBufferStorage(ssboOut,ssboOutSize,nullptr,InOutFlags);
    OutPtr = (float*)glMapNamedBufferRange(ssboOut,0,ssboOutSize,InOutFlags);
    memset(OutPtr,0,ssboOutSize);


    /* CREATING ID BUFFER */

    GLbitfield IdFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    GLsizeiptr ssboIdsSize = (GLsizeiptr)(bCapacity * sizeof(uint32_t));
    glCreateBuffers(1,&ssboIds);
    glNamedBufferStorage(ssboIds,ssboIdsSize,nullptr,IdFlags);
    idPtr = (uint32_t*)glMapNamedBufferRange(ssboIds,0,ssboIdsSize,IdFlags);



}

void SimulationEngine::InitNetworkData()
{
    Shader initShader = ResourceManager::GetShader("init");

    initShader.Use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboNetworks);
    initShader.SetInteger("stride", bCapacity);
    initShader.SetInteger("globalSeed", GameConfigData::getInt("seed"));

    auto DispatchInit = [&](int startParam, int count, float minVal,float maxVal) {
        initShader.SetInteger("paramOffset", startParam);
        initShader.SetInteger("paramCount", count);
        initShader.SetFloat("minVal", minVal);
        initShader.SetFloat("maxVal", maxVal);


        int groupsX = (bCapacity + 63) / 64;
        int groupsY = count;

        glDispatchCompute(groupsX, groupsY, 1);
    };
    auto DispatchInit2 = [&](int startParam, int count, float range) {
        initShader.SetInteger("paramOffset", startParam);
        initShader.SetInteger("paramCount", count);
        initShader.SetFloat("minVal", -range);
        initShader.SetFloat("maxVal", range);


        int groupsX = (bCapacity + 63) / 64;
        int groupsY = count;

        glDispatchCompute(groupsX, groupsY, 1);
    };


    int totalBiases = BIASES;
    DispatchInit(0, totalBiases, 0.0f,0.0f);

    int currentParamOffset = BIASES;

    float range1 = sqrt(6.0f / (INPUT + HIDDEN1));
    DispatchInit2(currentParamOffset, INPUT * HIDDEN1, range1);
    currentParamOffset += INPUT * HIDDEN1;


    float range2 = sqrt(6.0f / (HIDDEN1 + HIDDEN2));
    DispatchInit2(currentParamOffset, HIDDEN1 * HIDDEN2, range2);
    currentParamOffset += HIDDEN1 * HIDDEN2;


    float range3 = sqrt(6.0f / (HIDDEN2 + HIDDEN3));
    DispatchInit2(currentParamOffset, HIDDEN2 * HIDDEN3, range3);
    currentParamOffset += HIDDEN2 * HIDDEN3;


    float rangeOut = sqrt(6.0f / (HIDDEN3 + OUTPUT));
    DispatchInit2(currentParamOffset, HIDDEN3 * OUTPUT, rangeOut);


    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    std::cout << "Inicjalizacja na GPU zakonczona." << std::endl;
    size_t weightStartOffset = (size_t)BIASES * bCapacity;

    std::vector<float> debugWeights(10);

    glGetNamedBufferSubData(ssboNetworks, weightStartOffset * sizeof(float), 10 * sizeof(float), debugWeights.data());

    std::cout << "--- DEBUG WAG (Warstwa 1) ---" << std::endl;
    for(int i=0; i<10; i++) {
        std::cout << "Waga " << i << ": " << debugWeights[i] << std::endl;
    }
}




void SimulationEngine::Process(uint32_t id_size, uint32_t *ids, float* inputData, float* outputData)
{
    //for (int i=0;i<100;i++) std::cout << "WEJŚCIE: " << inputData[i] << std::endl;
    memcpy(idPtr,ids,id_size*sizeof(uint32_t));
    memcpy(InPtr, inputData, id_size * INPUT * sizeof(float));
    glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
    shader.Use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,ssboNetworks);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,ssboIn);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,ssboOut);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,3,ssboIds);
    shader.SetInteger("activeBacteria",bSize);
    shader.SetInteger("stride",bCapacity);
    shader.SetInteger("indices",id_size);
    shader.SetInteger("simStep", counter);
    shader.SetInteger("globalSeed",GameConfigData::getInt("seed"));
    glDispatchCompute((id_size + 63) / 64, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    GLenum result = glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
    glDeleteSync(fence);
    memcpy(outputData,OutPtr,id_size*OUTPUT*sizeof(float));

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "OpenGL Error: " << err << std::endl;
    }
}

void SimulationEngine::killNetwork(int deadIdx)
{
    int lastIdx = bSize - 1;

    if (deadIdx != lastIdx) {
        Shader killShader = ResourceManager::GetShader("kill");
        killShader.Use();
        killShader.SetInteger("stride", bCapacity);
        killShader.SetInteger("paramCount", SIZE);
        killShader.SetInteger("deadIdx", deadIdx);
        killShader.SetInteger("lastIdx", lastIdx);

        glDispatchCompute((SIZE + 63) / 64, 1, 1);
    }
    bSize--;
}

void SimulationEngine::reproduceNetwork(int parentA, int parentB, int childIdx)
{
    if (bSize >= bCapacity) return;

    Shader reproShader = ResourceManager::GetShader("reproduce");
    reproShader.Use();
    reproShader.SetInteger("stride", bCapacity);
    reproShader.SetInteger("paramCount", SIZE);
    reproShader.SetInteger("parentAIdx", parentA);
    reproShader.SetInteger("parentBIdx", parentB);
    reproShader.SetInteger("childIdx", childIdx);

    reproShader.SetFloat("mutationRate", 0.01f);
    reproShader.SetFloat("mutationChance", 0.05f);
    reproShader.SetInteger("simStep", counter);
    reproShader.SetInteger("globalSeed",GameConfigData::getInt("seed"));

    glDispatchCompute((SIZE + 63) / 64, 1, 1);

    bSize++;
}