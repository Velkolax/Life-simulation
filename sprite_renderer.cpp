#include "sprite_renderer.h"

#include "game.h"
#include "GLFW/glfw3.h"
#include "glm/common.hpp"
#include "glm/common.hpp"
#include "glm/common.hpp"
#include "glm/common.hpp"
#include "glm/common.hpp"
#include "glm/common.hpp"
#include "glm/common.hpp"



SpriteRenderer::SpriteRenderer(Shader &shader)
{
    this->shader = shader;
    this->initRenderData();
    this->residentData.resize(20);
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
}



void SpriteRenderer::addToDisplacementX(int dx)
{
    displacementX += dx;
}
void SpriteRenderer::addToDisplacementY(int dy)
{
    displacementY += dy;
}

void SpriteRenderer::addToResizeMultiplier(double ds,Board *board,float width)
{
    resizeMultiplier *= ds;
}

void SpriteRenderer::setBrightenedHexes(std::vector<Hexagon*> hexes)
{
    for (auto hex : hexes)
    {
        brightenedHexes.push_back(hex);
    }
}

void SpriteRenderer::ClearBrightenedHexes()
{
    brightenedHexes.clear();
}


std::vector<int> rand_vect(std::vector<int> base_vector)
{
    std::vector<int> temp_vector;
    int reps = base_vector.size();

    for (int i = 0; i < reps; i++)
    {
        std::uniform_int_distribution<> dis(0, base_vector.size() - 1);
        int random_number = dis(gen);

        temp_vector.push_back(base_vector[random_number]);
        base_vector.erase(base_vector.begin() + random_number);
    }
    return temp_vector;
}

void SpriteRenderer::InitPalette() {
    std::vector<int> hexColors{
        0xCC3333,
        0x33CC33,
        0x3333CC,
        0xCCCC33,
        0x33CCCC,
        0xCC33CC,
        0xCC6633,
        0x99CC33,
        0x3399CC,
        0x9933CC
    };
    std::vector<int> shuffled = rand_vect(hexColors);
    for (auto hex : shuffled)
    {

        double red, green, blue;
        red = hex >> 16 ;

        green = (hex & 0x00ff00) >> 8;

        blue = (hex & 0x0000ff);
        // std::cout << red << " " << green << " " << blue << std::endl;
        palette.push_back(glm::vec3(red/255.0f,green/255.0f,blue/255.0f));
    }
}

Point fromAxial(int q,int r)
{
    int parity = q&1;
    int col = q;
    int row = r + (q - parity) / 2;
    return Point(col, row);
}

Point SpriteRenderer::CheckWhichHexagon(int _x, int _y, float baseSize)
{

    float worldX = _x - this->displacementX;
    float worldY = _y - this->displacementY;

    float normalizedX = worldX;
    float normalizedY = worldY;

    float a = baseSize;

    float x = normalizedX - a;
    float y = normalizedY - 0.866 * a;

    float x2 = x / a;
    float y2 = y / a;

    int q = round(2./3 * x2);
    int r = round(-1./3 * x2 + sqrt(3)/3 * y2);

    return fromAxial(q, r);
}

void SpriteRenderer::Zoom(float zoomFactor, float pivotX, float pivotY)
{
    float oldZoom = resizeMultiplier;
    float newZoom = oldZoom * zoomFactor;

    if (newZoom < 0.5f) newZoom = 0.5f;
    // if (newZoom > 3.0f) newZoom = 3.0f;

    float scaleRatio = newZoom / oldZoom;
    displacementX = pivotX - (pivotX - displacementX) * scaleRatio;
    displacementY = pivotY - (pivotY - displacementY) * scaleRatio;

    resizeMultiplier = newZoom;
}

const float SQRT_3 = 1.7320508f;


glm::vec2 SpriteRenderer::calculateHexPosition(int gridX, int gridY, float size)
{
    float height = size * SQRT_3 / 2.0f;
    float posX = gridX * size * 0.75f + displacementX / resizeMultiplier;
    float posY = gridY * height + displacementY / resizeMultiplier;
    if (gridX % 2 != 0)
    {
        posY += height / 2.0f;
    }
    return glm::vec2(posX, posY);
}

glm::vec2 Jump(float size)
{
    float time = glfwGetTime();
    float speed = 3.0f;
    float pulse = (std::sin(time * speed) + 1.0f) / 2.0f * size / 5;
    return glm::vec2(0.0f,pulse);
}



float SpriteRenderer::getSize(Board *board)
{
    float boardWidth = static_cast<float>(board->getWidth());
    float screenWidth = static_cast<float>(width) * resizeMultiplier;
    return (screenWidth / boardWidth) / 0.75f;
}

void SpriteRenderer::RenderBatch(const std::string &textureName, const std::vector<HexInstanceData> &data)
{
    if (data.empty()) return;

    glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(HexInstanceData), data.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    ResourceManager::GetTexture(textureName).Bind();

    glBindVertexArray(this->quadVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, data.size());
    glBindVertexArray(0);
}


void SpriteRenderer::DrawBoard(Board *board, int width, int height, int playerIndex)
{

    this->shader.Use();

    glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    this->shader.SetMatrix4("projection", projection);


    this->shader.SetVector2f("viewOffset", glm::vec2(displacementX, displacementY));
    this->shader.SetFloat("zoom", resizeMultiplier);


    RenderBatch("hexagon", hexData);

    for (auto& r : warriorToTexture)
    {
        RenderBatch(r.second,residentData[(int)r.first]);
    }

}





void SpriteRenderer::initRenderData()
{
    // 1. Definicja geometrii pojedynczego quada (tak jak miałeś w oryginale)
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    unsigned int VBO; // To VBO dla geometrii (lokalne, bo dane są statyczne)

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    // --- KONFIGURACJA 1: GEOMETRIA (Quad) ---
    glBindVertexArray(this->quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Atrybut 0: vertex (vec4: x,y,u,v) - czytany z VBO
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // Ważne: NIE ustawiamy tu Divisor, bo to są dane per-wierzchołek!


    glGenBuffers(1, &this->instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(HexInstanceData) * 70000, nullptr, GL_DYNAMIC_DRAW);

    // Atrybut 1: Position (vec2) - czytany z instanceVBO
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HexInstanceData), (void*)offsetof(HexInstanceData, position));
    glVertexAttribDivisor(1, 1); // Dane zmieniają się co 1 instancję

    // Atrybut 2: Color (vec3)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(HexInstanceData), (void*)offsetof(HexInstanceData, color));
    glVertexAttribDivisor(2, 1);

    // Atrybut 3: Rotation (float)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(HexInstanceData), (void*)offsetof(HexInstanceData, rotation));
    glVertexAttribDivisor(3, 1);

    // Atrybut 4: Size (vec2)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(HexInstanceData), (void*)offsetof(HexInstanceData, size));
    glVertexAttribDivisor(4, 1);

    // Sprzątanie
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    this->InitPalette();
}
