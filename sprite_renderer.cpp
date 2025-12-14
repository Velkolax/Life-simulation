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

#include <algorithm>



SpriteRenderer::SpriteRenderer(Board *board)
{
    int bWidth = board->getWidth();
    int bHeight = board->getHeight();
    this->initRenderData(bWidth,bHeight);
    width=board->getGame()->Width;
    height=board->getGame()->Height;
    size = getSize(board);
    setActualDimensions(board);
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->quadVAO);
    glDeleteBuffers(1, &this->quadVBO);
}

void SpriteRenderer::setActualDimensions(Board *board)
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
    //std::cout << actualBoardWidth << " " << actualBoardHeight << std::endl;
}

void SpriteRenderer::addToDisplacementX(Board *board,int dx)
{
    displacementX += dx;
    glm::ivec2 pos1 = CheckWhichHexagon(width,0,size/2);
    glm::ivec2 pos2 = CheckWhichHexagon(0,height,size/2);
    // if (pos1.x<actualMinX) displacementX-=2*abs(dx);
    // if (pos2.x>actualMaxX) displacementX+=2*abs(dx);

}
void SpriteRenderer::addToDisplacementY(Board *board,int dy)
{
    displacementY += dy;
    glm::ivec2 pos1 = CheckWhichHexagon(width,0,size/2);
    glm::ivec2 pos2 = CheckWhichHexagon(0,height,size/2);
    // if (pos2.y<actualMinY) displacementY-=2*abs(dy);
    // if (pos1.y>actualMaxY) displacementY+=2*abs(dy);
}


void SpriteRenderer::addToResizeMultiplier(double ds,Board *board,float width)
{
    resizeMultiplier *= ds;
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

float SpriteRenderer::getSize(Board *board)
{
    float boardWidth = static_cast<float>(std::max(actualBoardWidth,actualBoardHeight));
    float screenWidth = static_cast<float>(width) * resizeMultiplier;
    return (screenWidth / boardWidth) / 0.75f;
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


void SpriteRenderer::DrawSprites(GLuint spriteSSBO,Board *board,std::string textureName,int index,Shader shader)
{
    shader.Use();
    glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    shader.SetMatrix4("projection", projection);
    shader.SetVector2f("viewDisplacement",glm::vec2(displacementX,displacementY));
    shader.SetFloat("hexSize",getSize(board));
    shader.SetInteger("bWidth",board->getWidth());
    shader.SetInteger("bHeight",board->getHeight());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, spriteSSBO);

    glActiveTexture(GL_TEXTURE0);
    ResourceManager::GetTexture(textureName).Bind();
    glBindVertexArray(this->quadVAO);
    glDrawArraysInstanced(GL_TRIANGLES,0,6,board->getHeight()*board->getWidth());
    glBindVertexArray(0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void SpriteRenderer::initRenderData(int bWidth,int bHeight)
{
    float vertices[] = {
        // pos      // tex
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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
