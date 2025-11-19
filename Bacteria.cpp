//
// Created by tk2 on 11/18/25.
//

#include "Bacteria.h"


Bacteria Bacteria::Crossover(const Bacteria &bacteria2) const {
    const auto bac = Bacteria(CrossOver(this->network,bacteria2.network),
        (this->lifeTime+bacteria2.lifeTime)/2,
        (this->energyLevel+bacteria2.energyLevel)/2,
        (this->maxEnergy+bacteria2.maxEnergy)/2,
        (this->upgradeLevel+bacteria2.upgradeLevel)/2,
        (this->venomLevel+bacteria2.venomLevel/2));
    return bac;
}

void Bacteria::Mutate() {

}

