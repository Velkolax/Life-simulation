#include "sprite_renderer.h"

#include "game.h"
#include "GLFW/glfw3.h"
#include "glm/common.hpp"

#include <algorithm>

#include "game_configdata.h"


SpriteRenderer::SpriteRenderer(Shader &shader,Board *board,int screenWidth, int screenHeight, Game *game)
{
    this->width = screenWidth;
    this->height = screenHeight;
    int bWidth = board->getWidth();
    int bHeight = board->getHeight();
    this->shader = shader;
    this->initRenderData(bWidth,bHeight);
    this->hexData.resize(bWidth*bHeight);
    this->residentData.resize(20);
    this->getActualDimensions(board);
    this->game = game;
    this->board = board;
    for (auto& r : residentData) r.resize(bWidth*bHeight);
    generatePalette(GameConfigData::getInt("clansCount"));
}

void SpriteRenderer::getActualDimensions(Board *board)
{
    int maxX = 0; int minX = board->getWidth()-1;
    int maxY = 0; int minY = board->getHeight()-1;
    for (int i=0;i<board->getWidth()*board->getHeight();i++)
    {
        Hexagon *h = board->getHexagon(i);
        if (!wall(h->getResident()))
        {
            if (h->getX()>maxX) maxX=h->getX();
            if (h->getX()<minX) minX=h->getX();
            if (h->getY()>maxY) maxY=h->getY();
            if (h->getY()<minY) minY=h->getY();
        }
    }
    actualMinX = minX;
    actualMaxX = maxX;
    actualMaxY = maxY;
    actualMinY = minY;
    actualBoardWidth=abs(maxX-minX);
    actualBoardHeight=abs(maxY-minY);
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
    glDeleteBuffers(1, &this->quadVBO);
    glDeleteBuffers(1,&this->instanceVBO);
}

void SpriteRenderer::addToDisplacementX(Board *board,int dx)
{
    displacementX += dx;
    glm::ivec2 pos1 = CheckWhichHexagon(width,0,size/2);
    glm::ivec2 pos2 = CheckWhichHexagon(0,height,size/2);
    if (pos1.x<actualMinX) displacementX-=2*abs(dx);
    if (pos2.x>actualMaxX) displacementX+=2*abs(dx);

}
void SpriteRenderer::addToDisplacementY(Board *board,int dy)
{
    displacementY += dy;
    glm::ivec2 pos1 = CheckWhichHexagon(width,0,size/2);
    glm::ivec2 pos2 = CheckWhichHexagon(0,height,size/2);
    if (pos2.y<actualMinY) displacementY-=2*abs(dy);
    if (pos1.y>actualMaxY) displacementY+=2*abs(dy);
}


void SpriteRenderer::addToResizeMultiplier(double ds,Board *board,float width)
{
    resizeMultiplier *= ds;
}

glm::vec3 SpriteRenderer::hsv_to_rgb(float h, float s, float v)
{
    int i = floor(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    float r, g, b;
    switch (i % 6) {
    case 0: r = v, g = t, b = p; break;
    case 1: r = q, g = v, b = p; break;
    case 2: r = p, g = v, b = t; break;
    case 3: r = p, g = q, b = v; break;
    case 4: r = t, g = p, b = v; break;
    case 5: r = v, g = p, b = q; break;
    }
    return {r, g, b};
}

void SpriteRenderer::generatePalette(int n)
{
    float h = 0.5;
    const float golden_ratio_conjugate = 0.618033988749895f;

    for (int i = 0; i < n; ++i) {
        palette.push_back(hsv_to_rgb(h, 0.6f, 0.9f));
        h += golden_ratio_conjugate;
        if (h > 1.0f) h -= 1.0f; // h = fmod(h, 1.0f)
    }
}

glm::ivec2 fromAxial(int q,int r)
{
    int parity = q&1;
    int col = q;
    int row = r + (q - parity) / 2;
    return glm::ivec2(col, row);
}

glm::ivec2 SpriteRenderer::CheckWhichHexagon(int _x, int _y, float baseSize)
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


void SpriteRenderer::Zoom(float zoomFactor, float pivotX, float pivotY,Board *board)
{
    float oldZoom = resizeMultiplier;
    float newZoom = oldZoom * zoomFactor;

    if (newZoom < 0.3f) newZoom = 0.3f;
    if (newZoom > std::max(actualBoardWidth,actualBoardHeight)/4) newZoom = std::max(actualBoardWidth,actualBoardHeight)/4;

    float scaleRatio = newZoom / oldZoom;

    displacementX = pivotX - (pivotX - displacementX) * scaleRatio;
    displacementY = pivotY - (pivotY - displacementY) * scaleRatio;

    resizeMultiplier = newZoom;
}

const float SQRT_3 = 1.7320508f;


glm::vec2 SpriteRenderer::calculateHexPosition(int gridX, int gridY, float size)
{
    float height = size * SQRT_3 / 2.0f;
    float posX = gridX * size * 0.75f + displacementX;
    float posY = gridY * height + displacementY;
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
bool SpriteRenderer::isHexOnScreen(glm::vec2 hexPos)
{
    return !(hexPos.x>width || hexPos.x<-size || hexPos.y > height || hexPos.y<-size);
}

float SpriteRenderer::getSize(Board *board)
{
    float boardWidth = static_cast<float>(std::max(actualBoardWidth,actualBoardHeight));
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

std::vector<int> SpriteRenderer::getAllIndicesOnAScreen(Board *board)
{
    std::vector<int> ind;
    glm::ivec2 p1 = CheckWhichHexagon(0,0,size);
    if (p1.x<0) p1.x=0; if (p1.y<0) p1.y=0;

    glm::ivec2 p2 = CheckWhichHexagon(width,height+size,size/2);
    if (p2.x>=board->getWidth()) p2.x=board->getWidth()-1; if (p2.y>=board->getHeight()) p2.y=board->getHeight()-1;
    for (int i=p1.y*board->getWidth()+p1.x;i<=p2.y*board->getWidth()+p2.x;i++)
    {
        if (i<board->getWidth()*board->getHeight() && i>=0)
        {
            ind.push_back(i);
        }
    }
    return ind;
}

void SpriteRenderer::generateSprites(Board *board)
{
    size = getSize(board);
    hexData.clear();
    for (auto& r : residentData) r.clear();

    std::vector<int> ind = getAllIndicesOnAScreen(board);
    for (int i =0;i<board->getWidth()*board->getHeight();i++)
    {
        Hexagon *hex = board->getHexagon(i);
        glm::vec2 hexSizeVec(size, size * 1.73 / 2.0f);
        float smallSize = size * 0.8;
        glm::vec2 smallSizeVec(smallSize, smallSize);

        glm::vec3 color = glm::vec3(1.0f,1.0f,1.0f);
        if (bacteria(hex->getResident()))
        {
            BacteriaData &bac = board->getBacteria(hex->getData().bacteriaIndex);
            color = palette[hex->getClan()-1];
            if (game->getInput().isToggled(GLFW_KEY_0)) color ={1.0f, 1.0f-((float)bac.energy/(float)MAX_STORED_VALUE),1.0f};
            else if (game->getInput().isToggled(GLFW_KEY_1)) color ={1.0f, 1.0f-((float)bac.protein/(float)MAX_STORED_VALUE),1.0f};
            else if (game->getInput().isToggled(GLFW_KEY_2)) color = {1.0f, (bac.lastAction==Action::Eat ? 0.2f : 0.7f),1.0f};
            else if (game->getInput().isToggled(GLFW_KEY_3)) color = {1.0f, 1.0f-((float)bac.age/(float)board->highestAge),1.0f};
            else if (game->getInput().isToggled(GLFW_KEY_4)) color = {1.0f,1.0f-((float)bac.mothered/5.0f),1.0f};
        }

        glm::vec2 hexPos = calculateHexPosition(hex->getX(), hex->getY(), size);
        glm::vec2 unitPos = hexPos + glm::vec2((size-smallSize)/2,0);

        if (!wall(hex->getResident())) hexData.push_back(HexInstanceData(hexPos,glm::vec3(1.0f),0.0f,hexSizeVec));
        residentData[(int)hex->getResident()].push_back({unitPos,color,0.0f,smallSizeVec});
    }
}


void SpriteRenderer::DrawBoard(Board *board, int width, int height)
{
    generateSprites(board);
    glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    this->shader.Use();
    this->shader.SetMatrix4("projection", projection);
    if (!game->getInput().isToggled(GLFW_KEY_L)) RenderBatch("hexagon", hexData);
    if (!game->getInput().isToggled(GLFW_KEY_K))
    {
        for (int i=0;i<(int)Resident::Bacteria;i++)
        {
            if (textures[i]!="nic" && !game->getInput().isToggled(GLFW_KEY_V) && !bacteria((Resident)i))
                RenderBatch(textures[i],residentData[i]);
        }
    }

    Shader &sh = ResourceManager::GetShader("instance_bac");
    sh.Use();
    sh.SetMatrix4("projection",projection);
    for (int i=(int)Resident::Bacteria;i<=(int)Resident::Bacteria+GameConfigData::getInt("clansCount");i++)
    {
        RenderBatch(textures[(int)Resident::Bacteria],residentData[i]);
    }

}





void SpriteRenderer::initRenderData(int bWidth,int bHeight)
{

    float vertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };


    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &this->quadVBO);

    glBindVertexArray(this->quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);


    glGenBuffers(1, &this->instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(HexInstanceData) * bWidth * bHeight, nullptr, GL_DYNAMIC_DRAW);


    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(HexInstanceData), (void*)offsetof(HexInstanceData, position));
    glVertexAttribDivisor(1, 1);


    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(HexInstanceData), (void*)offsetof(HexInstanceData, color));
    glVertexAttribDivisor(2, 1);


    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(HexInstanceData), (void*)offsetof(HexInstanceData, rotation));
    glVertexAttribDivisor(3, 1);


    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(HexInstanceData), (void*)offsetof(HexInstanceData, size));
    glVertexAttribDivisor(4, 1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}
