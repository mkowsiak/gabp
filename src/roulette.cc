#include <assert.h>
#include <stdlib.h>
#include "roulette.h"
#include "population.h"
#include "memory.h"


Roulette *createRoulette(Population *population)
{
	Roulette *roulette = (Roulette*)_malloc(sizeof(Roulette));
	assert(roulette != NULL);
	assert(population != NULL);
	if(roulette && population)
	{
		roulette->positions = population->individualsCnt;
		roulette->roulette = (double*)_malloc(sizeof(double)*population->individualsCnt);
		roulette->roulette[0] = population->individuals[0]->fittnes;
		for(unsigned i=1;i<roulette->positions;i++)
			roulette->roulette[i] = roulette->roulette[i-1] + population->individuals[i]->fittnes;
		return roulette;
	}
	return NULL;
}

unsigned getIndex(Roulette *roulette)
{
	double randVal = (double) rand() / (double)RAND_MAX  * roulette->roulette[roulette->positions-1];
	for(unsigned i=0;i<roulette->positions;i++)
		if(roulette->roulette[i] >= randVal)
			return i;
	return roulette->positions-1;
}

void clearRoulette(Roulette *roulette)
{
	if(roulette)
	{
		if(roulette->roulette)
		{
			roulette->positions = 0;	
			_free((void*)roulette->roulette);
		}
		_free((void*)roulette);
	}
}
