#include "game.h"

int main(int argc, char *argv[])
{
    Game* sim = new Game();
    sim->Run();
    delete sim;
    return 0;
}

