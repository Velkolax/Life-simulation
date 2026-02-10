#include "game.h"
#include "game_configdata.h"
#include <set>
#include <ft2build.h>
#include "BacteriaData.h"
#include  <random>
#include FT_FREETYPE_H
#include "resource_manager.h"
#include "simulation_engine.h"


unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 600;
bool fullScreen = false;
bool fPressed = false;


void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                            GLsizei length, const char* message, const void* userParam) {
    if (severity==GL_DEBUG_SEVERITY_MEDIUM)
    {
        std::cout << "OpenGL Warning Message: " << message << std::endl;
    }
    else std::cerr << "OpenGL Error Message: " << message << std::endl;
}

Game::Game() : Width(SCREEN_WIDTH), Height(SCREEN_HEIGHT), board()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, true);
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simulation", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) std::cout << "Failed to initialize GLAD" << std::endl;
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);

    ResourceManager::LoadShader({"shaders/instance.vs.glsl"},{"shaders/instance.fs.glsl"},"instance");
    ResourceManager::LoadShader({"shaders/instance.vs.glsl"},{"shaders/instance_bac.fs.glsl"},"instance_bac");
    ResourceManager::LoadComputeShader({"shaders/process_network.cs.glsl"},"network");
    ResourceManager::LoadComputeShader({"shaders/init_weights.cs.glsl"},"init");
    ResourceManager::LoadComputeShader({"shaders/kill.cs.glsl"},"kill");
    ResourceManager::LoadComputeShader({"shaders/reproduce.cs.glsl"},"reproduce");
    ResourceManager::LoadTexture("textures/square-16.png", true, "hexagon");
    ResourceManager::LoadTexture("textures/bacteria.png",true,"bacteria");
    ResourceManager::LoadTexture("textures/energy.png",true,"apple");
    ResourceManager::LoadTexture("textures/explosion.png",true,"explosion");
    ResourceManager::LoadTexture("textures/acid.png",true,"acid");
    ResourceManager::LoadTexture("textures/protein.png",true,"protein");

    GameConfigData::setConfigDataFromFile("config.txt");
    int seed = GameConfigData::getInt("seed");
    std::cout << "Seed: " << seed << std::endl;
    if (!seed)
    {
        std::mt19937 g{std::random_device{}()};
        std::cout << "AAA" << std::endl;
        GameConfigData::setInt("seed",std::to_string(std::uniform_int_distribution<int>{0,100000}(g)));
    }
    std::cout << "Seed: " << GameConfigData::getInt("seed") << std::endl;
    gen.seed(GameConfigData::getInt("seed"));
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load(24);
    int bacteriaCount = GameConfigData::getInt("bacteriaCount");
    int x = GameConfigData::getInt("width");
    int y = GameConfigData::getInt("height");
    board = new Board(x, y, this,bacteriaCount);
    board->InitializeNeighbour((x-2)/2, true);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("instance"),board,Width,Height,this);

    board->spawnBacteria();
    board->spawnFood(0.1);
    engine = new SimulationEngine(board);

}

void Game::restart()
{
    delete board;
    delete Renderer;
    delete Text;

    GameConfigData::setConfigDataFromFile("config.txt");
    int seed = GameConfigData::getInt("seed");
    if (!seed)
    {
        std::mt19937 g{std::random_device{}()};
        GameConfigData::setInt("seed",std::to_string(std::uniform_int_distribution<int>{0,100000}(g)));
    }

    Text = new TextRenderer(this->Width, this->Height);
    Text->Load(24);
    int bacteriaCount = GameConfigData::getInt("bacteriaCount");
    int x = GameConfigData::getInt("width");
    int y = GameConfigData::getInt("height");
    board = new Board(x, y, this,bacteriaCount);
    board->InitializeNeighbour((x-2)/2, true);
    board->spawnBacteria();
    board->spawnFood(0.1);
    engine->Restart(board);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("instance"),board,Width,Height,this);

}

Game::~Game()
{
    delete Renderer;
    ResourceManager::Clear();
    glfwTerminate();
}

void Game::Run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        input.update(window);
        this->ProcessInput();
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        this->Render();
        glfwSwapBuffers(window);
    }
}

void Game::tick()
{
    if (board->getAliveBacteriaCount()<=0) this->restart();
    for (int i=0;i<GameConfigData::getInt("substeps");i++) board->tick();
}

void Game::Resize(int width, int height)
{
    this->Width = width;
    this->Height = height;
    Text->TextShader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f), true);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("instance").Use().SetMatrix4("projection", projection);
}


void Game::ProcessInput()
{
    if (this->scroll != 0)
    {
        float zoomFactor = (this->scroll == 1) ? 1.1f : 0.9f;
        float centerX = this->Width / 2.0f;
        float centerY = this->Height / 2.0f;
        Renderer->Zoom(zoomFactor, centerX, centerY,board);
        scroll = 0;
    }
    if (input.isDown(GLFW_KEY_W)) Renderer -> addToDisplacementY(board,10);
    if (input.isDown(GLFW_KEY_A)) Renderer ->addToDisplacementX(board,10);
    if (input.isDown(GLFW_KEY_S)) Renderer -> addToDisplacementY(board,-10);
    if (input.isDown(GLFW_KEY_D)) Renderer -> addToDisplacementX(board,-10);

    if (input.isReleased(GLFW_KEY_ENTER)) this->tick();
    else if (input.isDown(GLFW_KEY_SPACE)) this->tick();
    else if (input.isToggled(GLFW_KEY_P)) this->tick();

    // if (input.isPressed(GLFW_KEY_F))
    // {
    //     fullscreen = !fullscreen;
    //     if (fullscreen) glfwSetWindowMonitor(window,glfwGetPrimaryMonitor(),0,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,GLFW_DONT_CARE);
    //     else glfwSetWindowMonitor(window,NULL,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,GLFW_DONT_CARE);
    // }

    if (input.isReleased(GLFW_KEY_R)) this->restart();
}



void Game::Render()
{
    Renderer -> DrawBoard(board,Width,Height);
    if (mousePressed)
    {
        float size = Renderer -> getSize(board);
        glm::ivec2 p = Renderer -> CheckWhichHexagon(cursorPosX,cursorPosY,size/2);
        Hexagon *hex = board->getHexagon(p.y*board->getWidth()+p.x);
        if (hex!=nullptr && bacteria(hex->getResident()))
        {
            ResidentData res = hex->getData();
            BacteriaData bac = board->getBacteria(res.bacteriaIndex);
            Text->RenderText("AGE: "+ std::to_string(bac.age),10,10,1.0);
            Text->RenderText("ACID: "+ std::to_string(bac.acid),10,40,1.0);
            Text->RenderText("ENERGY: "+ std::to_string(bac.energy),10,70,1.0);
            Text->RenderText("PROTEIN: "+std::to_string(bac.protein),10,100,1.0);
            Text->RenderText("SPEED: " + std::to_string(bac.speed),10,130,1.0);
            Text->RenderText("LIFESPAN: " + std::to_string(bac.lifespan),10,160,1.0);
            Text->RenderText("LAST ACTION: "+stringActions[(int)bac.lastAction],10,190,1.0);
            Text->RenderText("MOTHRED: "+std::to_string(bac.mothered),10,220,1.0);
            Text->RenderText("FATHERED: "+std::to_string(bac.fathered),10,250,1.0);
            Text->RenderText("SPECIES: "+std::to_string(hex->getClan()),10,280,1.0);
            Text->RenderText("POPULATION: "+std::to_string(board->getNumberOfMembersOfSpecies(hex->getClan())),10,310,1.0);
        }
        if (hex!=nullptr && protein(hex->getResident()))
        {
            ProteinData res = hex->getData().protein;
            auto a = res.amount;
            Text->RenderText("PROTEIN AMOUNT: "+ std::to_string(a),10,10,1.0 );
        }
        if (hex!=nullptr && energy(hex->getResident()))
        {
            EnergyData res = hex->getData().energy;
            auto a = res.amount;
            Text->RenderText("ENERGY AMOUNT: "+ std::to_string(a),10,10,1.0 );
        }
        if (hex!=nullptr && acid(hex->getResident()))
        {
            AcidData res = hex->getData().acid;
            auto a = res.amount;
            Text->RenderText("ACID AMOUNT: "+ std::to_string(a),10,10,1.0 );
        }
    }

    Text->RenderText("NUMBER OF BACTERIA: "+std::to_string(board->getAliveBacteriaCount()),Width*0.5,10,1.0);
    Text->RenderText("STEP: "+std::to_string(board->getStep()),Width*0.5,40,1.0);
    Text->RenderText("SPECIES LEFT: "+std::to_string(board->getAliveSpeciesNumber()),Width*0.5,70,1.0);

}

void Game::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

void Game::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        game-> mousePressed = true;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        game -> cursorPosX = xpos;
        game -> cursorPosY = ypos;
    }
    else if(action == GLFW_RELEASE) game -> mousePressed = false;

}

void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (width!=game->Width || height!=game->Height)
    {
        glViewport(0, 0, width, height);
        game->Resize(width, height);
    }

}

void Game::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if(yoffset==-1) game->scroll = -1;
    else if(yoffset==1) game->scroll = 1;
}

