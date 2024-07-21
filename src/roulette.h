#ifndef _ROULETTE_H
#define _ROULETTE_H

#include "population.h"

struct Roulette
{
	unsigned positions;
	double *roulette;
};

Roulette *createRoulette(Population*);
Roulette *addIndex(Population*);
void clearRoulette(Roulette *);
unsigned getIndex(Roulette*);

#endif