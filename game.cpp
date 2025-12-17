#include "game.h"

#include <set>

#include <ft2build.h>

#include "BacteriaData.h"

#include FT_FREETYPE_H

#include "resource_manager.h"
#include "simulation_engine.h"


// Game-related State data
SpriteRenderer* Renderer;
SimulationEngine* Engine;


Game::Game(unsigned int width, unsigned int height) : State(GameState::GAME_ACTIVE), Width(width), Height(height), board()
{
}

Game::~Game()
{
    delete Renderer;
}

void Game::Init()
{
    ResourceManager::LoadShader({"shaders/instance.vs.glsl"},{"shaders/instance.fs.glsl"},"instance");
    ResourceManager::LoadComputeShader({"shaders/common.glsl","shaders/movement.cs.glsl"},"movement");
    ResourceManager::LoadComputeShader({"shaders/common.glsl","shaders/pass_time.cs.glsl"},"passTime");
    ResourceManager::LoadTexture("textures/square-16.png", true, "hexagon");
    ResourceManager::LoadTexture("textures/bacteria.png",true,"bacteria");
    ResourceManager::LoadTexture("textures/apple.png",true,"apple");
    ResourceManager::LoadTexture("textures/explosion.png",true,"explosion");

    Text = new TextRenderer(this->Width, this->Height);
    Text->Load(24);
    int bacteriaCount = 1000;
    int x = 100;
    int y = 100;
    board = new Board(x, y, this);
    int total = x*y;
    board->InitializeNeighbour(249, true);
    board->spawnBacteria(bacteriaCount);
    board->spawnFood(0.1);


    Engine = new SimulationEngine(board);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("instance"),board);
}






void Game::Update(float dt)
{
    Engine->Tick([&](DataInOut* ptr)
    {
        for (int i=0;i<NUMBER_OF_ACTIVE_BACTERIA;i++)
        {
            if (Engine->tickCounter>0)
            {
                float a = ptr[i].output[0];
                float b = ptr[i].output[1];
                std::cout << "OUT1: " << a << " OUT2: " << b << std::endl;
            }
            ptr[i].input[0] = 5;
        }
    });
}

void Game::Resize(int width, int height)
{
    this->Width = width;
    this->Height = height;
    Text->TextShader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f), true);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", projection);
}



/*void Game::Tick()
{
    //board->moveBacteriasRandomly();
    //board->passTime();
    //board->tick();
    //step+=1;
}*/


void Game::ProcessInput(float dt)
{
    if (this->scroll != 0)
    {
        float zoomFactor = (this->scroll == 1) ? 1.1f : 0.9f;
        float centerX = this->Width / 2.0f;
        float centerY = this->Height / 2.0f;

        Renderer->Zoom(zoomFactor, centerX, centerY,board);

        scroll = 0;
    }
    if (clickedMovingKeys[GLFW_KEY_W])
    {
        Renderer -> addToDisplacementY(board,10);
    }
    if (clickedMovingKeys[GLFW_KEY_A])
    {
        Renderer ->addToDisplacementX(board,10);
    }
    if (clickedMovingKeys[GLFW_KEY_S])
    {
        Renderer -> addToDisplacementY(board,-10);
    }
    if (clickedMovingKeys[GLFW_KEY_D])
    {
        Renderer -> addToDisplacementX(board,-10);
    }
    if (pressedKey==GLFW_KEY_SPACE)
    {
        //this->Tick();
    }
    if(pressedKey==GLFW_KEY_ENTER)
    {
        enterPressed = true;

    }
    if(pressedKey!=GLFW_KEY_ENTER && enterPressed)
    {
        //enterPressed = false;
        //this->Tick();
    }

}



void Game::Render()
{
    Renderer -> DrawBoard(board,Width,Height);
}
