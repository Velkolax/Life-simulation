#include <catch2/catch_test_macros.hpp>
#include "../board.h"
#include "../BacteriaData.h"
#include "../game_configdata.h"
#include "../simulation_engine.h"
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QSurfaceFormat>

#include "../resource_manager.h"

TEST_CASE("Board initialization", "[board]")
{
    GameConfigData::setConfigDataFromFile("../config.txt");
    Board board(50,60,nullptr,10);

    SECTION("Board dimensions are correct")
    {
        REQUIRE(board.getWidth() == 50);
        REQUIRE(board.getHeight() == 60);
    }
}

TEST_CASE("Hexagon neighbor logic", "[grid]")
{
    GameConfigData::setConfigDataFromFile("../config.txt");
    Board board(20,20,nullptr,0);

    SECTION("Even row neighbors")
    {
        Hexagon *h = board.getHexagon(2,2);
        auto neighbors = h->neighbours(&board,0,false);
        REQUIRE(neighbors.size() == 6);
    }
}

TEST_CASE("Bacterium metabolism and death", "[bacteria]")
{
    GameConfigData::setConfigDataFromFile("../config.txt");
    Board board(10,10,nullptr,0);
    Hexagon *hex = board.getHexagon(5,5);
    hex->placeBacteriaC(&board,1);

    int id = hex->getData().bacteriaIndex;
    BacteriaData& data = board.getBacteria(id);

    SECTION("Bacterium gains energy from eating")
    {
        data.energy = 10;
        Hexagon* foodHex = board.getHexagon(5,6);
        foodHex->placeEnergy(20);

        float mockData[] = { 0.0f, 1.0f, 0.0f };
        data.eat(&board,foodHex,mockData,5,5);
        REQUIRE(data.energy>10);
        REQUIRE(foodHex->getResident() == Resident::Empty);
    }
}

TEST_CASE("Bacterium breeding logic","[bacteria]")
{
    GameConfigData::setConfigDataFromFile("../config.txt");
    Board board(10,10,nullptr,0);
    board.InitializeNeighbour(4, true);
    Hexagon *momHex = board.getHexagon(5,5);
    momHex->placeBacteriaC(&board,1);
    int momId = momHex->getData().bacteriaIndex;
    BacteriaData& momData = board.getBacteria(momId);

    Hexagon* dadHex = board.getHexagon(5,4);
    dadHex->placeBacteriaC(&board,1);

    SECTION("Successful breeding")
    {
        momData.energy = 100;
        momData.protein = 100;


        float mockData[] = {0.1f,0.5f,0.3f,0.3f};
        int result = momData.breed(&board,dadHex,mockData,5,5);
        REQUIRE(result==1);
        auto neighbors = momHex->neighbours(&board,2,false);
        int bacteriaCount = 0;
        for (auto n : neighbors)
        {
            if (bacteria(n->getResident())) bacteriaCount++;
        }
        REQUIRE(momData.energy<100);
        REQUIRE(momData.protein<100);
        REQUIRE(bacteriaCount==2);
    }

    SECTION("Fail breeding due to no protein")
    {
        momData.energy = 100;
        momData.protein = 1;
        float mockData[] = {0.1f, 0.5f, 0.3f, 0.3f};

        momData.breed(&board, dadHex, mockData,5,5);
        REQUIRE(momData.lastAction == Action::BreedFailureNoProtein);
    }
}


TEST_CASE("Board resource merging", "[board]")
{
    GameConfigData::setConfigDataFromFile("../config.txt");
    Board board(10,10,nullptr,0);
    board.InitializeNeighbour(4, true);

    Hexagon* h1 = board.getHexagon(5,5);
    Hexagon* h2 = board.getHexagon(5,6);

    h1->placeProtein(10);
    h2->placeProtein(20);
    board.resourcesMerge();

    bool merged = (h1->getData().protein.amount == 30 && h2->getResident() == Resident::Empty ||
                    h2->getData().protein.amount == 30 && h1->getResident() == Resident::Empty);
    REQUIRE(merged);
}

TEST_CASE("Bacterium attack logic","[bacteria]")
{
    GameConfigData::setConfigDataFromFile("../config.txt");
    Board board(10,10,nullptr,0);
    board.InitializeNeighbour(4, true);

    Hexagon* attackerHex = board.getHexagon(5,5);
    attackerHex->placeBacteriaC(&board,1);
    BacteriaData& attackerData = board.getBacteria(attackerHex->getData().bacteriaIndex);

    Hexagon* victimHex = board.getHexagon(5,6);
    victimHex->placeBacteriaC(&board,2);
    BacteriaData& victimData = board.getBacteria(victimHex->getData().bacteriaIndex);

    attackerData.acid = 80;
    attackerData.energy = 100;
    victimData.energy = 50;

    float mockData[] = {0.1f,0.5f,0.0f};
    attackerData.attack(&board,victimHex,mockData,5,5);
    REQUIRE(attackerData.acid < 80);
    REQUIRE(victimData.energy < 50);
    REQUIRE(attackerData.lastAction == Action::Attack);
}

struct OpenGLTestContext
{
    QGuiApplication* app;
    QOpenGLContext* context;
    QOffscreenSurface* surface;

    OpenGLTestContext()
    {
        static int argc = 1;
        static char* argv[] = {(char*)"test"};
        if (!qApp) app = new QGuiApplication(argc,argv);
        else app = qApp;

        QSurfaceFormat format;
        format.setVersion(4,5);
        format.setProfile(QSurfaceFormat::CoreProfile);
        QSurfaceFormat::setDefaultFormat(format);

        context = new QOpenGLContext();
        context->setFormat(format);
        context->create();

        surface = new QOffscreenSurface();
        surface->setFormat(format);
        surface->create();
        context->makeCurrent(surface);
    }

    ~OpenGLTestContext()
    {
        context->doneCurrent();
        delete surface;
        delete context;
    }
};

TEST_CASE("SimulationEngine Inference Process","[engine]")
{
    OpenGLTestContext glContext;
    GameConfigData::setConfigDataFromFile("../config.txt");
    ResourceManager::LoadComputeShader({"../shaders/process_network.cs.glsl"},"network");
    ResourceManager::LoadComputeShader({"../shaders/init_weights.cs.glsl"},"init");

    Board board(20,20,nullptr,1);
    board.getHexagon(0)->placeBacteriaC(&board,1);
    SimulationEngine engine(&board);

    const int numBacteria = 2;
    uint32_t ids[] = {0,1};

    std::vector<float> input(numBacteria * INPUT, 1.0f);
    std::vector<float> output(numBacteria * OUTPUT,1.0f);

    SECTION("Data round-trip via GPU")
    {
        engine.Process(numBacteria, ids, input.data(),output.data());
        bool allZeros = true;
        for (float val : output)
        {
            if (val!=0.0f)
            {
                allZeros = false;
                break;
            }
        }
        REQUIRE_FALSE(allZeros);

        for (float val : output)
            REQUIRE_FALSE(std::isnan(val));
    }
}

TEST_CASE("SimulationEngine Network Lifecycle","[engine]")
{
    OpenGLTestContext glContext;
    GameConfigData::setConfigDataFromFile("../config.txt");
    ResourceManager::LoadComputeShader({"../shaders/process_network.cs.glsl"},"network");
    ResourceManager::LoadComputeShader({"../shaders/init_weights.cs.glsl"},"init");
    ResourceManager::LoadComputeShader({"../shaders/reproduce.cs.glsl"},"reproduce");
    ResourceManager::LoadComputeShader({"../shaders/kill.cs.glsl"},"kill");

    Board board(20,20,nullptr,1);
    board.getHexagon(0,0)->placeBacteriaC(&board,1);
    board.getHexagon(0,1)->placeBacteriaC(&board,1);
    SimulationEngine engine(&board);

    int initialSize = engine.getbSize();
    REQUIRE(initialSize==2);

    SECTION("Network Reproduction")
    {
        int childIdx = initialSize;
        engine.reproduceNetwork(0,1,childIdx);
        REQUIRE(engine.getbSize() == initialSize+1);
        REQUIRE(glGetError() == GL_NO_ERROR);
    }

    SECTION("Network Termination")
    {
        int sizeBefore = engine.getbSize();
        engine.killNetwork(0);
        REQUIRE(engine.getbSize() == sizeBefore -1);
        REQUIRE(glGetError() == GL_NO_ERROR);
    }
}




