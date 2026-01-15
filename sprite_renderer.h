#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "texture.h"
#include "shader.h"
#include "resource_manager.h"
#include <cstdlib>


struct HexInstanceData {
    glm::vec2 position;
    glm::vec3 color;
    float rotation;
    glm::vec2 size;
};

inline std::string textures[] = {
    "nic",
    "nic",
    "bacteria",
    "acid",
    "apple",
    "protein",
};


class Board;
class Game;
class Hexagon;

class SpriteRenderer
{
public:
    SpriteRenderer(Shader &shader, Board *board,int screenWidth, int screenHeight,Game *game);
    void getActualDimensions(Board* board);
    ~SpriteRenderer();
    void constrainMapBounds(Board* board);

    std::vector<int> getAllIndicesOnAScreen(Board* board);
    void generateSprites(Board* board);
    void generateBorders(Board* board);
    void DrawBoard(Board* board, int width, int height);


    bool isHexOnScreen(glm::vec2 hexPos);
    float getSize(Board* board);
    void RenderBatch(const std::string& textureName, const std::vector<HexInstanceData>& data);
    glm::ivec2 CheckWhichHexagon(int x, int y, float size);
    glm::ivec2 CheckWhichActualHexagon(int _x, int _y, float baseSize);
    void Zoom(float zoomFactor, float pivotX, float pivotY, Board* board);
    glm::vec2 calculateHexPosition(int gridX, int gridY, float size);
    void InitPalette(Board *board);

    void addToDisplacementX(Board *board,int dx);
    void addToDisplacementY(Board *board,int dy);
    void addToResizeMultiplier(double ds, Board* board, float width);
    void setBrightenedHexes(std::vector<Hexagon*> hexes);
    void ClearBrightenedHexes();

    float size;
    std::vector<HexInstanceData> hexData;
    std::vector<std::vector<HexInstanceData>> residentData;
    int   displacementX = 0;
    int   displacementY = 0;
    int   actualBoardWidth=0;
    int   actualBoardHeight=0;
    int actualMinX=0;
    int actualMaxX =0;
    int actualMinY = 0;
    int actualMaxY = 0;
    double resizeMultiplier = 1.0f;
    int width;
    int height;
private:
    Shader       shader;
    unsigned int quadVAO;
    unsigned int instanceVBO;
    unsigned int quadVBO;
    Board *board;
    Game *game;
    void initRenderData(int bWidth, int bHeight);
};

#endif