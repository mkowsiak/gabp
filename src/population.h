#ifndef _POPULATION_H
#define _POPULATION_H

#include "individual.h"

struct Population
{
	Individual **individuals;
	int individualsCnt;
	int width;
	int lastAdapted;
	double mutation;
	double crossover;
	double avgFitt;
	double maxFitt;
	double minFitt;
};


Population *createPopulation(int , double , double);
void clearPopulation(Population *);
Population *addIndividual(Population *);
Population *addIndividual(Population *,Individual *);
Population *crossOver(Population *,Individual *, Individual *);
double calculatePopulationFittnes(Population *);
Population *clonePopulation(Population *);
Population *mutatePopulation(Population*);
Population *adaptIndividual(Population *,Individual *);
void printPopulation(Population *);
void logPopulation(Population *);

#endif