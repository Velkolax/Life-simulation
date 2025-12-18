#include <charconv>
#include <iostream>

#include "NeuralNetwork.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdfloat>
#include "game.h"
#include "resource_manager.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// The Width of the screen
unsigned int SCREEN_WIDTH = 800;
// The height of the screen
unsigned int SCREEN_HEIGHT = 600;
bool fullScreen = false;
bool fPressed = false;



Game* sim = new Game(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char *argv[])
{
    unsigned int seed = std::random_device{}();
    gen = std::mt19937(seed);
    Seed = seed;

    float a = 0.5;
    float b = 0.6;
    uint f = glm::packHalf2x16(glm::vec2(a,b));
    glm::uvec4 g = glm::uvec4(f,f,f,f);
    glm::vec2 e = glm::unpackHalf2x16(g.x);
    std::cout << e.x << " " << e.y << std::endl;


    // OPENGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simulation", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // FPS zsynchronizowany z odświeżaniem monitora (zazwyczaj 60 FPS)

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);


    // OpenGL configuration
    // --------------------
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // initialize game
    // ---------------
    sim->Init();
    // deltaTime variables
    // -------------------
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // calculate delta time
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        // manage user input
        // -----------------
        sim->ProcessInput(deltaTime);

        // update game state
        // -----------------
        sim->Update(deltaTime);

        // render
        // ------
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        sim->Render();
        counter++;
        glfwSwapBuffers(window);
    }

    // delete all resources as loaded using the resource manager
    // ---------------------------------------------------------
    ResourceManager::Clear();
    delete sim;
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_F && action == GLFW_PRESS && !fullScreen && !fPressed)
    {
        glfwSetWindowMonitor(window,glfwGetPrimaryMonitor(),0,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->width,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,GLFW_DONT_CARE);
        fPressed = true;
    }
    else if (key == GLFW_KEY_F && action == GLFW_RELEASE && !fullScreen && fPressed){fPressed=false;fullScreen=true;}
    else if (key == GLFW_KEY_F && action == GLFW_PRESS && fullScreen && !fPressed)
    {
        glfwSetWindowMonitor(window,NULL,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,GLFW_DONT_CARE);
        fPressed = true;
    }
    else if (key == GLFW_KEY_F && action == GLFW_RELEASE && fullScreen && fPressed){fullScreen=false;fPressed=false;}
    // std::cout << "PRESSED: " << fPressed << " FULLSCREEN: " << fullScreen << std::endl;
    if (key > 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            if (sim->pressedKey==-1)
                sim->pressedKey = key;
            if (sim->clickedMovingKeys.contains(key))
            {
                sim->clickedMovingKeys[key]=true;
            }
        }
        else if (action == GLFW_RELEASE)
        {
            sim->pressedKey = -1;
            if (sim->clickedMovingKeys.contains(key))
            {
                sim->clickedMovingKeys[key]=false;
            }
        }

    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        sim -> mousePressed = true;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        sim -> cursorPosX = xpos;
        sim -> cursorPosY = ypos;
    }
    else if(action == GLFW_RELEASE){
        sim -> mousePressed = false;
    }

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    if (width!=sim->Width || height!=sim->Height)
    {
        glViewport(0, 0, width, height);
        sim->Resize(width, height);
    }

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // std::cout << "SCROLL: " << xoffset << " " << yoffset << std::endl;
    if(yoffset==-1)
    {
        sim->scroll = -1;
    }
    else if(yoffset==1)
    {
        sim->scroll = 1;
    }
}