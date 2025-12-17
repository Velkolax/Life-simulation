#include "game.h"

#include <set>

#include <ft2build.h>

#include "BacteriaData.h"

#include FT_FREETYPE_H

#include "resource_manager.h"
#include "simulation_engine.h"


// Game-related State data
SpriteRenderer* Renderer;


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
    ResourceManager::LoadComputeShader({"shaders/process_network.cs.glsl"},"network");
    ResourceManager::LoadComputeShader({"shaders/init_weights.cs.glsl"},"init");
    ResourceManager::LoadTexture("textures/square-16.png", true, "hexagon");
    ResourceManager::LoadTexture("textures/bacteria.png",true,"bacteria");
    ResourceManager::LoadTexture("textures/apple.png",true,"apple");
    ResourceManager::LoadTexture("textures/explosion.png",true,"explosion");

    Text = new TextRenderer(this->Width, this->Height);
    Text->Load(24);
    int bacteriaCount = 30000;
    int x = 300;
    int y = 300;
    board = new Board(x, y, this,bacteriaCount);
    int total = x*y;
    board->InitializeNeighbour(249, true);
    board->spawnBacteria(bacteriaCount);
    board->spawnFood(0.1);


    engine = new SimulationEngine(board);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("instance"),board,Width,Height);
}






void Game::Update(float dt)
{
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
        spacePressed = true;
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
    if(pressedKey!=GLFW_KEY_SPACE && spacePressed)
    {
        if (!isPaused) isPaused=true;
        else isPaused=false;
    }

}



void Game::Render()
{
    Renderer -> DrawBoard(board,Width,Height);
    if (mousePressed)
    {
        float size = Renderer -> getSize(board);
        glm::ivec2 p = Renderer -> CheckWhichHexagon(cursorPosX,cursorPosY,size/2);
        Hexagon *hex = board->getHexagon(p.x,p.y);
        if (hex!=nullptr)
        {
            BacteriaData bac = board->getBacteria(p.y*board->getWidth()+p.x);
            Text->RenderText("AGE: "+std::to_string(bac.age),0,0,1.0);
            Text->RenderText("ENERGY: "+std::to_string(bac.energy),0,30,1.0);
        }
    }
    Text->RenderText("NUMBER OF BACTERIA: "+std::to_string(board->getBacteriaCount()),Width*0.75,0,1.0);
    Text->RenderText("GENERATION: "+std::to_string(counter),Width*0.75,0,1.0);
}
