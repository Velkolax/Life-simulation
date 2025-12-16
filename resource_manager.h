#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include <map>
#include <string>

#include <glad/glad.h>
#include "stb_image.h"
#include "texture.h"
#include "shader.h"


// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no 
// public constructor is defined.
class ResourceManager
{
public:
    // resource storage
    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture2D> Textures;
    // loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
    static Shader&    LoadShader(std::vector<const char*> vShaderFiles,std::vector<const char*> fShaderFiles, std::string name);
    static Shader& LoadComputeShader(std::vector<const char*> cShaderFiles, std::string name);
    static Shader& LoadShader(std::string name);
    static Shader& LoadShaderText(std::string name);
    // retrieves a stored sader
    static Shader&    GetShader(std::string name);
    // loads (and generates) a texture from file
    static Texture2D& LoadTexture(const char *file, bool alpha, std::string name);
    // retrieves a stored texture
    static Texture2D& GetTexture(std::string name);
    // properly de-allocates all loaded resources
    static void      Clear();
private:
    // private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
    ResourceManager() { }
    // loads and generates a shader from file
    static Shader    loadShaderFromFile(std::vector<const char*> vShaderFiles, std::vector<const char*> fShaderFiles);
    static Shader    loadComputeShaderFromFile(std::vector<const char*> cShaderFiles);
    static Shader loadDefaultShader();
    static Shader loadTextShader();
    // loads a single texture from file
    static Texture2D loadTextureFromFile(const char *file, bool alpha);
};

#endif