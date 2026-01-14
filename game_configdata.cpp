#include "game_configdata.h"
#include <iostream>
#include <algorithm>

std::unordered_map<std::string, std::string> GameConfigData::configMap;

void GameConfigData::setConfigDataFromFile(std::string filename)
{
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file,line))
    {
        line.erase(std::remove(line.begin(),line.end(),' '),line.end());
        if (line.empty()) continue;
        size_t pos = line.find('=');
        if (pos!=std::string::npos)
        {
            std::string key = line.substr(0,pos);
            std::string value = line.substr(pos+1);
            configMap[key]=value;
        }
    }
}

int GameConfigData::getInt(const std::string& key, int defaultValue)
{
    if (configMap.contains(key)) return std::stoi(configMap[key]);
    std::cerr << "Nie ma takiego klucza w configu!!!" << std::endl;
    exit(1);
}

void GameConfigData::setInt(const std::string& key, const std::string& value)
{
    if (configMap.contains(key)) configMap[key]=value;
    else
    {
        std::cerr << "Nie ma takiego klucza w configu!!!" << std::endl;
        exit(1);
    }
}

float GameConfigData::getFloat(const std::string& key, float defaultValue)
{
    if (configMap.contains(key)) return std::stof(configMap[key]);
    std::cerr << "Nie ma takiego klucza w configu!!!" << std::endl;
    exit(1);
}

