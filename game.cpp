#include "game.h"

#include <set>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "resource_manager.h"


// Game-related State data
SpriteRenderer* Renderer;



Game::Game(unsigned int width, unsigned int height)
    : State(GameState::GAME_ACTIVE), Width(width), Height(height), board() {
}

Game::~Game()
{
    delete Renderer;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("shaders/instance.vs","shaders/instance.fs",nullptr,"sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // load textures
    ResourceManager::LoadTexture("textures/soilder1_256.png",true,"soilder1");
    ResourceManager::LoadTexture("textures/soldier2_256.png",true,"soilder2");
    ResourceManager::LoadTexture("textures/square-16.png", true, "hexagon");
    ResourceManager::LoadTexture("textures/placeholder.png",true,"placeholder");
    ResourceManager::LoadTexture("textures/exclamation.png",true,"exclamation");
    ResourceManager::LoadTexture("textures/castle_256.png",true,"castle");
    ResourceManager::LoadTexture("textures/pineTree_512.png",true,"pine");
    ResourceManager::LoadTexture("textures/palmTree_256.png",true,"palm");
    ResourceManager::LoadTexture("textures/tower_512.png",true,"tower");
    ResourceManager::LoadTexture("textures/gravestone_512.png",true,"gravestone");
    ResourceManager::LoadTexture("textures/shield_placeholder.png",true,"shield_placeholder");
    ResourceManager::LoadTexture("textures/b.png",true,"border_placeholder");
    ResourceManager::LoadTexture("textures/bacteria.png",true,"bacteria");
    ResourceManager::LoadTexture("textures/apple.png",true,"apple");

    Text = new TextRenderer(this->Width, this->Height);
    Text->Load(24);
    int bacteriaCount = 1000;
    board = new Board(100, 100, this);
    int total = 100*100;
    board->InitializeRandom(total * 0.5, total * 0.9);

    board->spawnBacteria(100);
    board->spawnFood(0.1);
    Renderer->width = Width;
    Renderer->height = Height;
    Renderer->size = Renderer->getSize(board);

    RefreshSprites();

}

void Game::Update(float dt)
{
    // getPlayer odejmuje od podanego indeksu 1 co jest kluczowe (id graczy numerowane są od 1), analogiczna do getCountry()
    // getPlayer(board->getCurrentPlayerId())->act();
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

        Renderer->Zoom(zoomFactor, centerX, centerY);

        scroll = 0;
    }
    if (clickedMovingKeys[GLFW_KEY_W])
    {
        Renderer -> addToDisplacementY(10);
    }
    if (clickedMovingKeys[GLFW_KEY_A])
    {
        Renderer ->addToDisplacementX(10);
    }
    if (clickedMovingKeys[GLFW_KEY_S])
    {
        Renderer -> addToDisplacementY(-10);
    }
    if (clickedMovingKeys[GLFW_KEY_D])
    {
        Renderer -> addToDisplacementX(-10);
    }
}

int Game::GetSelectedCastleReserves()
{
    std::unordered_map<Hexagon*, int>& m= board->getCountry(board->getCurrentPlayerId())->getCastles();
    int sum=0;
    if (provinceSelector!=nullptr)
    {
        for (auto a : m)
        {
            if (provinceSelector->province(board)[0]==a.first)
            {
                sum=a.second;
                break;
            }
        }
    }
    return sum;
}

int Game::GetSelectedCastleIncome()
{
    int sum=0;
    if (provinceSelector!=nullptr)
    {
        sum=provinceSelector->province(board)[0]->calculateProvinceIncome(board);
    }
    return sum;
}

void Game::RefreshSprites()
{

    float savedDispX = Renderer->displacementX;
    float savedDispY = Renderer->displacementY;
    float savedResize = Renderer->resizeMultiplier;


    Renderer->displacementX = 0.0f;
    Renderer->displacementY = 0.0f;
    Renderer->resizeMultiplier = 1.0f;

    Renderer->size = Renderer->getSize(board);

    Renderer->hexData.clear();
    Renderer->exclamationData.clear();
    for (auto& r : Renderer->residentData) r.clear();

    for (int i = 0; i < board->getWidth(); i++)
    {
        for (int j = 0; j < board->getHeight(); j++)
        {
            Hexagon *hex = board->getHexagon(j,i);
            glm::vec2 hexSizeVec(Renderer->size, Renderer->size);
            float smallSize = Renderer->size * 0.8;
            glm::vec2 smallSizeVec(smallSize, smallSize);
            glm::vec3 color = glm::vec3(1.0f,1.0f,1.0f);

            if (hex->getOwnerId()!=0) {
                color = Renderer->palette[hex->getOwnerId()%10];
            }
            if (auto it = std::ranges::find(Renderer->brightenedHexes,hex);it!=Renderer->brightenedHexes.end())
            {
                color -= glm::vec3(0.2,0.2,0.2);
            }
            glm::vec2 hexPos = Renderer->calculateHexPosition(hex->getX(), hex->getY(), Renderer->size) +(hexSizeVec * 0.5f);
            if (!water(hex->getResident())) Renderer -> hexData.push_back({hexPos,color,0.0f,hexSizeVec});

            Renderer -> residentData[(int)hex->getResident()].push_back({hexPos,glm::vec3(1.0f),0.0f,hexSizeVec});
            if (castle(hex->getResident())) Renderer->exclamationData.push_back({hexPos,glm::vec3(1.0f),0.0f,hexSizeVec});
            if (unmovedWarrior(hex->getResident())) Renderer->exclamationData.push_back({hexPos,glm::vec3(1.0f),0.0f,hexSizeVec});
        }
    }
    RefreshOutline();
    Renderer->displacementX = savedDispX;
    Renderer->displacementY = savedDispY;
    Renderer->resizeMultiplier = savedResize;

    // Przywróć poprawny rozmiar dla reszty logiki gry
    Renderer->size = Renderer->getSize(board);

}

std::vector<std::pair<coord, coord>> evenD =
{
    { 0, -1}, // górny
    {-1, -1}, // lewy górny
    {-1,  0}, // lewy dolny
    { 0,  1}, // dolny
    { 1,  0}, // prawy dolny
    { 1, -1}  // prawy górny
};

std::vector<std::pair<coord, coord>> oddD =
{
    { 0, -1}, // górny
    {-1,  0}, // lewy górny
    {-1,  1}, // lewy dolny
    { 0,  1}, // dolny
    { 1,  1}, // prawy dolny
    { 1,  0}  // prawy górny
};

std::vector<glm::vec2> getCenters(float a,glm::vec2 start)
{
    return std::vector<glm::vec2>{
            {glm::vec2(a,0.0f)+start},
            {glm::vec2(0.25*a,0.433*a)+start},
            {glm::vec2(0.25*a,1.299*a)+start},
            {glm::vec2(a,1.732*a)+start},
            {glm::vec2(1.75 *a,1.299*a)+start},
            {glm::vec2(1.75 * a,0.433*a)+start},
        };
}

void Game::RefreshOutline()
{

    Renderer->borderData.clear();
    if (provinceSelector!=nullptr)
    {
        float size = Renderer->size;
        std::vector<Hexagon*> hexes = provinceSelector->province(board);
        std::vector<float> rotations = {0.0f,120.0f,60.0f,0.0f,120.0f,60.0f};
        for (auto& hex : hexes)
        {
            auto& directions = (hex->getX() % 2 == 0) ? evenD : oddD;
            int i=0;
            for (auto [dx, dy] : directions)
            {
                Hexagon* n = board->getHexagon(hex->getX() + dx, hex->getY() + dy);
                if(n == nullptr || n->getOwnerId()!=board->getCurrentPlayerId())
                {
                    float width = size * 0.07;
                    float a = size/2;
                    std::vector<glm::vec2> centers = getCenters(a,Renderer->calculateHexPosition(hex->getX(),hex->getY(),size));
                    glm::vec3 color = Renderer->palette[hex->getOwnerId()%10];
                    color -= glm::vec3(0.25,0.25,0.25);
                    Renderer->borderData.push_back({centers[i],color,rotations[i],glm::vec2(a,width)});
                }
                i++;
            }
        }
    }

}

void Game::Render()
{
    Renderer -> DrawBoard(board, this->Width, this->Height,board->getCurrentPlayerId());
    board->moveBacteriasRandomly();
    board->passTime();
    RefreshSprites();
}
