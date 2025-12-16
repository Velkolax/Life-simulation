#include "resource_manager.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shaders-arr/sprite_vs.h"
#include "shaders-arr/sprite_fs.h"
#include "shaders-arr/text_vs.h"
#include "shaders-arr/text_fs.h"
// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;


Shader& ResourceManager::LoadShader(std::vector<const char*> vShaderFiles,std::vector<const char*> fShaderFiles, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFiles, fShaderFiles);
    return Shaders[name];
}
Shader& ResourceManager::LoadComputeShader(std::vector<const char*> cShaderFiles,std::string name)
{
    Shaders[name] = loadComputeShaderFromFile(cShaderFiles);
    return Shaders[name];
}

// Shader& ResourceManager::LoadShader(std::string name)
// {
//     Shaders[name] = loadDefaultShader();
//     return Shaders[name];
// }

// Shader& ResourceManager::LoadShaderText(std::string name)
// {
//     Shaders[name] = loadTextShader();
//     return Shaders[name];
// }

Shader& ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture2D& ResourceManager::LoadTexture(const char *file, bool alpha, std::string name)
{
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D& ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

void ResourceManager::Clear()
{
    // (properly) delete all shaders	
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    // (properly) delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(std::vector<const char*> vShaderFiles, std::vector<const char*> fShaderFiles)
{
    std::vector<std::string> vShaderContents;
    std::vector<const char*> vShaderCodes;
    std::vector<GLint> vlengths;
    // 1. retrieve the vertex/fragment source code from filePath
    for (auto vShaderFile : vShaderFiles)
    {
        std::string computeCode;
        try
        {
            // open files
            std::ifstream computeShaderFile(vShaderFile);
            std::stringstream cShaderStream;
            // read file's buffer contents into streams
            cShaderStream << computeShaderFile.rdbuf();
            // close file handlers
            computeShaderFile.close();
            // convert stream into string
            computeCode = cShaderStream.str();
        }
        catch (std::exception e)
        {
            std::cout << "ERROR::SHADER: Failed to read compute shader files" << std::endl;
        }
        vShaderContents.push_back(computeCode);

    }

    for (const auto& content : vShaderContents)
    {
        vShaderCodes.push_back(content.c_str());
        vlengths.push_back((GLint)content.length());
    }
    std::vector<std::string> fShaderContents;
    std::vector<const char*> fShaderCodes;
    std::vector<GLint> flengths;
    // 1. retrieve the vertex/fragment source code from filePath
    for (auto fShaderFile : fShaderFiles)
    {
        std::string computeCode;
        try
        {
            // open files
            std::ifstream computeShaderFile(fShaderFile);
            std::stringstream cShaderStream;
            // read file's buffer contents into streams
            cShaderStream << computeShaderFile.rdbuf();
            // close file handlers
            computeShaderFile.close();
            // convert stream into string
            computeCode = cShaderStream.str();
        }
        catch (std::exception e)
        {
            std::cout << "ERROR::SHADER: Failed to read compute shader files" << std::endl;
        }
        fShaderContents.push_back(computeCode);

    }

    for (const auto& content : fShaderContents)
    {
        fShaderCodes.push_back(content.c_str());
        flengths.push_back((GLint)content.length());
    }
    GLint vs = vShaderCodes.size();
    GLint fs = fShaderCodes.size();
    Shader shader;
    shader.Compile(vShaderCodes.data(),fShaderCodes.data(),vlengths.data(),flengths.data(),vs,fs);
    return shader;
}

Shader ResourceManager::loadComputeShaderFromFile(std::vector<const char*> cShaderFiles)
{
    std::vector<std::string> cShaderContents;
    std::vector<const char*> cShaderCodes;
    std::vector<GLint> lengths;
    // 1. retrieve the vertex/fragment source code from filePath
    for (auto cShaderFile : cShaderFiles)
    {
        std::string computeCode;
        try
        {
            // open files
            std::ifstream computeShaderFile(cShaderFile);
            std::stringstream cShaderStream;
            // read file's buffer contents into streams
            cShaderStream << computeShaderFile.rdbuf();
            // close file handlers
            computeShaderFile.close();
            // convert stream into string
            computeCode = cShaderStream.str();
        }
        catch (std::exception e)
        {
            std::cout << "ERROR::SHADER: Failed to read compute shader files" << std::endl;
        }
        cShaderContents.push_back(computeCode);

    }

    for (const auto& content : cShaderContents)
    {
        cShaderCodes.push_back(content.c_str());
        lengths.push_back((GLint)content.length());
    }
    GLint s = cShaderCodes.size();
    Shader shader;
    shader.CompileCompute(cShaderCodes.data(),lengths.data(),s);
    return shader;
}

// Shader ResourceManager::loadDefaultShader()
// {
//     Shader shader;
//     shader.Compile(shaders_sprite_vs,shaders_sprite_fs,nullptr);
//     return shader;
// }

// Shader ResourceManager::loadTextShader()
// {
//     Shader shader;
//     shader.Compile(shaders_text_vs,shaders_text_fs);
//     return shader;
// }


Texture2D ResourceManager::loadTextureFromFile(const char *file, bool alpha)
{
    // create texture object
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    // now generate texture
    texture.Generate(width, height, data);
    // and finally free image data
    stbi_image_free(data);
    return texture;
}