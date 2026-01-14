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



int main(int argc, char *argv[])
{
    Game* sim = new Game();
    sim->Run();
    delete sim;
    return 0;
}

