#include "game.h"

#include <set>

#include <ft2build.h>

#include "BacteriaData.h"

#include FT_FREETYPE_H

#include "resource_manager.h"
#include "simulation_engine.h"


// Game-related State data
SpriteRenderer* Renderer;


Game::Game(unsigned int width, unsigned int height) : Width(width), Height(height), board()
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
    ResourceManager::LoadComputeShader({"shaders/kill.cs.glsl"},"kill");
    ResourceManager::LoadComputeShader({"shaders/reproduce.cs.glsl"},"reproduce");
    ResourceManager::LoadTexture("textures/square-16.png", true, "hexagon");
    ResourceManager::LoadTexture("textures/bacteria.png",true,"bacteria");
    ResourceManager::LoadTexture("textures/apple.png",true,"apple");
    ResourceManager::LoadTexture("textures/explosion.png",true,"explosion");
    ResourceManager::LoadTexture("textures/acid.png",true,"acid");
    ResourceManager::LoadTexture("textures/protein.png",true,"protein");

    Text = new TextRenderer(this->Width, this->Height);
    Text->Load(24);
    int bacteriaCount = 70000;
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
    if (pressedKey==GLFW_KEY_SPACE)
    {
        board->tick();
        //if (counter % 100 == 0) board->spawnFood(0.05);
        //board->resourcesMerge();
        //board->proteinMerge();
        //board->energyMerge();
        //board->acidMerge();
        //counter++;

        // for (int i=0;i<board->getAliveBacteriaCount();i++)
        // {
        //     BacteriaData &bac = board->getBacteria(i);
        //     bac.printBacteria();
        // }
    }


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
    if (pressedKey==GLFW_KEY_ENTER) enterPressed = true;
    if (pressedKey!=GLFW_KEY_ENTER && enterPressed)
    {
        enterPressed=false;
        board->tick();
        //board->proteinMerge();
        //board->energyMerge();
        //counter++;
        //if (counter % 100 == 0) board->spawnFood(0.05);
    }

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
            Text->RenderText("ENERGY: "+ std::to_string(bac.energy),10,40,1.0);
            Text->RenderText("ACID: "+ std::to_string(bac.acid),10,70,1.0);
            Text->RenderText("SPEED: " + std::to_string(bac.speed),10,100,1.0);
            Text->RenderText("LAST ACTION: "+stringActions[(int)bac.lastAction],10,130,1.0);
            Text->RenderText("PROTEIN: "+std::to_string(bac.protein),10,160,1.0);
            //bac.printBacteria();
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
    // Text->RenderText("GENERATION: "+std::to_string(counter),Width*0.5,40,1.0);
    //
    // Text->RenderText("PROTEIN NUMBER: " + std::to_string(board->getProteinCount()),Width*0.5,70,1.0);
    // Text->RenderText("PROTEIN SHORTAGE: " + std::to_string(board->proteinShortage),Width*0.5,100,1.0);
    // Text->RenderText("HIGHEST AGE: " + std::to_string(board->getHighestAge()),Width*0.5,130,1.0);
    // Text->RenderText("LOWEST AGE: "+ std::to_string(board->getLowestAge()),Width*0.5,160,1.0);
    Text->RenderText("FAILURE RATIO: "+ std::to_string(board->getFailureRatio()),Width*0.5,310,1.0);
    Text->RenderText("SLEEP PERCENTAGE: " + std::to_string(board->getActionPercentage(Action::Sleep)),Width*0.5,340,1.0);
    Text->RenderText("BREED_FAILURE PERCENTAGE: "+std::to_string(board->getActionPercentage(Action::BreedFailure)),Width*0.5,370,1.0);
    Text->RenderText("BREED PERCENTAGE: "+std::to_string(board->getActionPercentage(Action::Breed)),Width*0.5,400,1.0);
}