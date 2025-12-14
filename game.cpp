#include "game.h"

#include <set>

#include <ft2build.h>

#include "BacteriaData.h"

#include FT_FREETYPE_H

#include "resource_manager.h"


// Game-related State data
SpriteRenderer* Renderer;
SpriteRenderer* Renderer_2;



Game::Game(unsigned int width, unsigned int height) : State(GameState::GAME_ACTIVE), Width(width), Height(height), board()
{
}

Game::~Game()
{
    delete Renderer;
}

void Game::Init()
{
    ResourceManager::LoadShader("shaders/render_resident.vs.glsl","shaders/render_resident.fs.glsl",nullptr,"sprite");
    ResourceManager::LoadShader("shaders/render_hex.vs.glsl","shaders/render_resident.fs.glsl",nullptr,"hex");
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
    board->InitializeRandom(total * 0.5, total * 0.9);

    board->spawnBacteria(bacteriaCount);
    board->spawnFood(0.1);
    Renderer = new SpriteRenderer(board);
    InitSsbos();
}

void Game::ssbo_barrier() {
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glMemoryBarrier.xhtml
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Game::InitSsbos()
{
    int boardSize = board->getHeight()*board->getWidth();
    std::vector<BacteriaData> b;
    std::vector<uint32_t> freePlaces(boardSize,0);
    std::vector<int32_t> boardData(boardSize, 0);


    int id = 0;
    for (int i=0;i<boardSize;i++)
    {
        Hexagon *h = board->getHexagon(i);
        if (bacteria(h->getResident()))
        {
            b.emplace_back(h->getPos(),id,1,100);
            boardData[i]=id+1;
            id++;
        }
    }
    for (int i=id;i<boardSize;i++)
    {
        b.emplace_back(glm::ivec2(-2137,-2137),id,0,0);
        freePlaces.push_back(i);
    }
    Counters counters;
    counters.aliveCount = id;
    counters.stackTop = boardSize-id;

    for (int i=0;i<boardSize;i++)
    {
        Hexagon *h = board->getHexagon(i);
        if (wall(h->getResident())) boardData[i]=-1;
    }

    auto createSSBO = [](GLuint& id, int binding, const void* data, size_t size) {
        glGenBuffers(1, &id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    };

    for (int i=0;i<boardData.size();i++) std::cout << boardData[i] << std::endl;
    createSSBO(ssboGrid,      1, boardData.data(), boardData.size() * sizeof(int32_t));
    createSSBO(ssboBacteria,    0, b.data(),    b.size() * sizeof(BacteriaData));

    createSSBO(ssboFreeList,  2, freePlaces.data(),  freePlaces.size() * sizeof(uint32_t));
    createSSBO(ssboCounters,  3, &counters,        sizeof(Counters));

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
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



void Game::Tick()
{
    //board->moveBacteriasRandomly();
    //board->passTime();
    //board->tick();
    //step+=1;
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
    if (pressedKey==GLFW_KEY_SPACE)
    {
        this->Tick();
    }
    if(pressedKey==GLFW_KEY_ENTER)
    {
        enterPressed = true;

    }
    if(pressedKey!=GLFW_KEY_ENTER && enterPressed)
    {
        enterPressed = false;
        this->Tick();
    }

}

// std::vector<std::pair<coord, coord>> evenD =
// {
//     { 0, -1}, // górny
//     {-1, -1}, // lewy górny
//     {-1,  0}, // lewy dolny
//     { 0,  1}, // dolny
//     { 1,  0}, // prawy dolny
//     { 1, -1}  // prawy górny
// };
//
// std::vector<std::pair<coord, coord>> oddD =
// {
//     { 0, -1}, // górny
//     {-1,  0}, // lewy górny
//     {-1,  1}, // lewy dolny
//     { 0,  1}, // dolny
//     { 1,  1}, // prawy dolny
//     { 1,  0}  // prawy górny
// };

// std::vector<glm::vec2> getCenters(float a,glm::vec2 start)
// {
//     return std::vector<glm::vec2>{
//             {glm::vec2(a,0.0f)+start},
//             {glm::vec2(0.25*a,0.433*a)+start},
//             {glm::vec2(0.25*a,1.299*a)+start},
//             {glm::vec2(a,1.732*a)+start},
//             {glm::vec2(1.75 *a,1.299*a)+start},
//             {glm::vec2(1.75 * a,0.433*a)+start},
//         };
// }



void Game::Render()
{

    Renderer->DrawSprites(ssboGrid,board,"hexagon",1,ResourceManager::GetShader("hex"));
    Renderer->DrawSprites(ssboBacteria,board,"bacteria",0,ResourceManager::GetShader("sprite"));
    // Renderer -> DrawBoard(board, this->Width, this->Height,board->getCurrentPlayerId());
    // Text->RenderText("KROK: "+std::to_string(step),10,10,1.0f);
    // Text->RenderText("LICZBA BAKTERII: "+std::to_string(board->getBacterias().size()),10,40,1.0f);
    // if (board->getBacterias().empty())
    // {
    //     Text->RenderText("KONIEC SYMULACJI",Width/2,10,1.0f);
    // }
    // RefreshSprites();
}
