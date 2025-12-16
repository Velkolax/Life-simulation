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
    ResourceManager::LoadShader({"shaders/common.glsl","shaders/render_resident.vs.glsl"},{"shaders/render_resident.fs.glsl"},"sprite");
    ResourceManager::LoadShader({"shaders/common.glsl","shaders/render_hex.vs.glsl"},{"shaders/render_resident.fs.glsl"},"hex");
    ResourceManager::LoadComputeShader({"shaders/common.glsl","shaders/movement.cs.glsl"},"movement");
    ResourceManager::LoadComputeShader({"shaders/common.glsl","shaders/pass_time.cs.glsl"},"passTime");
    ResourceManager::LoadTexture("textures/square-16.png", true, "hexagon");
    ResourceManager::LoadTexture("textures/bacteria.png",true,"bacteria");
    ResourceManager::LoadTexture("textures/apple.png",true,"apple");
    ResourceManager::LoadTexture("textures/explosion.png",true,"explosion");

    // Text = new TextRenderer(this->Width, this->Height);
    // Text->Load(24);
    int bacteriaCount = 1000;
    int x = 100;
    int y = 100;
    board = new Board(x, y, this);
    int total = x*y;
    board->InitializeNeighbour(249, true);
    //board->InitializeRandom(total * 0.5, total * 0.9);

    board->spawnBacteria(bacteriaCount);
    board->spawnFood(0.1);
    Engine = new SimulationEngine(board);
    Renderer = new SpriteRenderer(board);
}






void Game::Update(float dt)
{
    Engine->Tick();
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

    //double t1 = glfwGetTime();
    Renderer->DrawSprites(Engine->GetGridSSBO(),board,"hexagon",1,ResourceManager::GetShader("hex"));
    //double t2 = glfwGetTime();
    Renderer->DrawSprites(Engine->GetBacteriaSSBO(),board,"bacteria",0,ResourceManager::GetShader("sprite"));

    //double t3 = glfwGetTime();
    //double t4 = glfwGetTime();
    // std::cout << "CZAS RENDEROWANIA HEXÓW: " << t2-t1 << std::endl;
    // std::cout << "CZAS RENDEROWANIA BAKTERII: " << t3-t2 << std::endl;
    // std::cout << "CZAS OBLICZANIA: " << t4-t3 << std::endl;
    // Renderer -> DrawBoard(board, this->Width, this->Height,board->getCurrentPlayerId());
    // Text->RenderText("KROK: "+std::to_string(step),10,10,1.0f);
    // Text->RenderText("LICZBA BAKTERII: "+std::to_string(board->getBacterias().size()),10,40,1.0f);
    // if (board->getBacterias().empty())
    // {
    //     Text->RenderText("KONIEC SYMULACJI",Width/2,10,1.0f);
    // }
    // RefreshSprites();
}
