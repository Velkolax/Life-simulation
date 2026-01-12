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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 600;
bool fullScreen = false;
bool fPressed = false;



Game* sim = new Game(SCREEN_WIDTH, SCREEN_HEIGHT);

int main(int argc, char *argv[])
{
    unsigned int seed = std::random_device{}();
    gen = std::mt19937(seed);
    Seed = seed;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simulation", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    sim->Init();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        sim->ProcessInput(deltaTime);
        for (int i=0;i<1;i++)
        {
            sim->Update(deltaTime);
        }



        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        sim->Render();
        counter++;
        glfwSwapBuffers(window);
    }

    ResourceManager::Clear();
    delete sim;
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
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