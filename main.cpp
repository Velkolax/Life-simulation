#include "NeuralNetwork.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "LifeSimulator.h"

int main()
{
    /*srand((unsigned int)time(NULL));
    int layers[] = {2, 3, 2};
    int layerCount = sizeof(layers)/sizeof(layers[0]);
    NeuralNetwork nn1 = buildNetwork(layerCount, layers);
    initializeRandom(&nn1);
    printNetwork(&nn1);
    printf("\n");
    NeuralNetwork nn2 = buildNetwork(layerCount, layers);
    initializeRandom(&nn2);
    printNetwork(&nn2);
    printf("\n");
    
    NeuralNetwork child1 = childNetwork(&nn1, &nn2, 0); // brak mutacji
    printNetwork(&child1);
    printf("\n");
    
    NeuralNetwork child2 = childNetwork(&nn1, &nn2, 0.2); // mała mutacja
    printNetwork(&child2);
    printf("\n");
    
    freeNetwork(&nn1);
    freeNetwork(&nn2);
    freeNetwork(&child1);
    freeNetwork(&child2);
    
    getchar();*/
    auto sim = LifeSimulator(100);
    sim.Run();
    return 0;
}