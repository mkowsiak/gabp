#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "individual.h"
#include "monitor.h"
#include "population.h"
#include "network_test.h"
#include "roulette.h"
#include "memory.h"
#include "log.h"

Population *createPopulation(int width, double mutation, double crossover)
{
	Population *population = (Population*)_malloc(sizeof(Population));
	assert(population != NULL);
	if(population)
	{
		population->individuals = NULL;
		population->width = width;
		population->lastAdapted = 0;
		population->avgFitt = 0;
		population->minFitt = 0;
		population->maxFitt = 0;
		population->mutation = mutation;
		population->crossover = crossover;
		population->individualsCnt = 0;
		return population;
	}
	else
		return NULL;
}

void clearPopulation(Population *population)
{
	if(population)
	{
		for(int i=0;i<population->individualsCnt;i++)
			clearIndividual(population->individuals[i]);
		population->individualsCnt = 0;
		_free((void*)population->individuals);
		_free((void*)population);
	}
}

Population *addIndividual(Population *population)
{
	if(population)
	{
		population->individuals = (Individual**)_realloc(population->individuals,sizeof(Individual*)*(population->individualsCnt+1));
		assert(population->individuals != NULL);
		Individual *individual = createIndividual(population->width);
		assert(individual != NULL);
		if(population->individuals && individual)
		{
				population->individuals[population->individualsCnt] = individual;
				population->individualsCnt ++;
				return population;
		}
		else
			_free(individual);
	}		
	return NULL;
}


Population *addIndividual(Population *population,Individual *individual)
{
	assert(individual != NULL);
	assert(population != NULL);
	if(population && individual)
	{
		population->individuals = (Individual**)_realloc(population->individuals,sizeof(Individual*)*(population->individualsCnt+1));
		assert(population->individuals != NULL);
		if(population->individuals)
		{
				population->individuals[population->individualsCnt] = individual;
				population->individualsCnt ++;
				return population;
		}
	}		
	return NULL;
}

Population *crossOver(Population *population,Individual *individualA, Individual *individualB)
{
	int crossPnt;
	assert(individualA != NULL);
	assert(individualB != NULL);
	if(!individualA || !individualB)
		return NULL;
	if(individualA->dataWidth == individualB->dataWidth)
	{
		Individual *newA=createIndividual(population->width);
		assert(newA != NULL);
		Individual *newB=createIndividual(population->width);	
		assert(newB != NULL);
		if(!newA || !newB)
		{
			clearIndividual(newA);
			clearIndividual(newB);
			return NULL;	
		}
		crossPnt = rand() % individualA->dataWidth;
		for(int i=0;i<crossPnt;i++)
		{
			newA->genotype[i] = individualA->genotype[i];
			newB->genotype[i] = individualB->genotype[i];
		}
		for(int i=crossPnt;i<individualA->dataWidth;i++)
		{
			newA->genotype[i] = individualB->genotype[i];
			newB->genotype[i] = individualA->genotype[i];
		}
		if(!addIndividual(population,newA))
		{
			clearIndividual(newA);
			clearIndividual(newB);
			return NULL;
		}
		if(!addIndividual(population,newB))
		{
			clearIndividual(newB);
			return NULL;
		}
		else
			return population;
	}	
	return NULL;	
}

int compare(const void *a,const void *b)
{
	if((*(Individual**)a)->fittnes < (*(Individual**)b)->fittnes)
		return -1;
	else
	{
		if((*(Individual**)a)->fittnes > (*(Individual**)b)->fittnes)
			return 1;
		else
			return 0;
	}
}



double calculatePopulationFittnes(Population *population)
{
	double sumFit = 0;
	double fit = 0;
	int 	popSize =  population->individualsCnt;
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	assert(population != NULL);
	if(population && pp)
	{
		for(int i=0;i<popSize;i++)
		{
			double fitSum = 0;
			for(int p=0;p<pp->gap.gaFitCnt;p++)
			{
				fit = calculateIndividualFittnes(population->individuals[i]);
				if(fit == 0)
					return 0;
				fitSum += fit;
			}
			fitSum /= (double)pp->gap.gaFitCnt;
			population->individuals[i]->fittnes = fitSum;
			sumFit += fitSum;
		}
		qsort(population->individuals,popSize,sizeof(Individual*),compare);

		if(pp->gap.gaFitnessByPosition)
			for(int i=0;i<popSize;i++)
				population->individuals[i]->fittnes = i;
		
		assert(popSize != 0);
		if(popSize != 0)
		{
 			population->maxFitt = population->individuals[popSize - 1]->fittnes;
			population->minFitt = population->individuals[0]->fittnes;
			population->avgFitt = sumFit / (double) popSize;	
		}
		
		/* Normalizacja wartosci przystosowania */
		
		if(pp->gap.gaNormalization)
		{
			double left = pp->gap.gaLeftNorm;
			double right = pp->gap.gaRightNorm;
			if(pp->gap.gaLeftNorm < 0 || pp->gap.gaRightNorm < 0 || pp->gap.gaLeftNorm >= pp->gap.gaRightNorm)
			{
				left	= 0;
				right	= 1;
			}
		
			if(right - left == 0)
				LOG(LOG_ERROR_COMPUTE_ERROR,"division by zero within population fitness");
			else
			{
				sumFit = 0;
				for(int i=0;i<popSize;i++)
				{
					population->individuals[i]->fittnes =
						(right-left)*
						((population->individuals[i]->fittnes-population->minFitt)/
						(population->maxFitt-population->minFitt))+left;
					sumFit += population->individuals[i]->fittnes;
				}
				
				assert(popSize!=0);
				if(popSize != 0)
				{
					population->maxFitt = population->individuals[popSize - 1]->fittnes;
					population->minFitt = population->individuals[0]->fittnes;
					population->avgFitt = sumFit / (double) popSize;	
				}
			}
		}

		
		return population->avgFitt;
	}
	return 0;
}

Population *clonePopulation(Population *population)
{
	Roulette *roulette;
	Individual *a;
	Individual *b;
	Population *_new;

	if(population)
	{
		roulette = createRoulette(population);
		assert(roulette != NULL);
		if(!roulette)
			return NULL;
		
		_new = createPopulation(population->width,population->mutation,population->crossover);
		assert(_new != NULL);
		if(!_new)
		{
			clearRoulette(roulette);
			return NULL;
		}
		_new->avgFitt = population->avgFitt;
		_new->minFitt = population->minFitt;
		_new->maxFitt = population->maxFitt;
		_new->lastAdapted = 0;
		
		for(int i=0;i<(population->individualsCnt)/2;i++)
		{
			int aIndex = getIndex(roulette);
			int bIndex = getIndex(roulette);
			//printf("index1: %d, index2: %d\n",aIndex,bIndex);
			a=population->individuals[aIndex];
			b=population->individuals[bIndex];
			double cross = (double)rand()/(double)RAND_MAX;
			if(cross <= population->crossover)
			{
				if(!crossOver(_new,a,b))
				{
					clearPopulation(_new);
					clearRoulette(roulette);
					return NULL;
				}
			}
			else
			{
				Individual *cloneA = cloneIndividual(a);
				Individual *cloneB = cloneIndividual(b);
				assert(cloneA != NULL);
				assert(cloneB != NULL);
				if(cloneA && cloneB)
				{
					if(!addIndividual(_new,cloneA) || !addIndividual(_new,cloneB))
					{
						clearIndividual(cloneA);
						clearIndividual(cloneB);
						clearPopulation(_new);
						clearRoulette(roulette);
						return NULL;
					}
				}
				else
				{
					clearIndividual(cloneA);
					clearIndividual(cloneB);
					clearPopulation(_new);
					clearRoulette(roulette);
					return NULL;
				}
			}
		}
		if((population->individualsCnt)%2)
		{
			int aIndex = getIndex(roulette);
			
			a=population->individuals[aIndex];
			Individual *cloneA = cloneIndividual(a);
			assert(cloneA != NULL);
			
			if(cloneA)
			{
				if(!addIndividual(_new,cloneA))
				{
						clearIndividual(cloneA);
						clearPopulation(_new);
						clearRoulette(roulette);
						return NULL;
				}
			}
			else
			{
				clearPopulation(_new);
				clearRoulette(roulette);
				return NULL;
			}
		}
		clearRoulette(roulette);
		return _new;
	}
	else
		return NULL;
}

Population *adaptIndividual(Population *population,Individual *individual)
{
	assert(population != NULL);
	assert(individual != NULL);
	if(population && individual)
	{
		if(population->lastAdapted < population->individualsCnt)
		{
			Individual *toRemove = population->individuals[population->lastAdapted];
			population->individuals[population->lastAdapted] = individual;
			clearIndividual(toRemove);
			population->lastAdapted ++;
		}
		else
			clearIndividual(individual);
		return population;
	}
	else
		return NULL;
}

Population *mutatePopulation(Population *population)
{
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	assert(population != NULL);
	if(population && pp)
	{
		unsigned indiSize = 0;
		if(population->individuals)
			indiSize = population->individuals[0]->dataWidth;
		for(int i=0;i<population->individualsCnt;i++)
			for(unsigned int p=0;p<indiSize;p++)
				if( (double) rand() / (double) RAND_MAX <= pp->gap.gaMutation)
				{
					// tutaj tak samo pozycja genotypu musi byc zmieniana w odpowiedni sposob
					// printf("Mutacja");
					population->individuals[i]->genotype[p].pos = !population->individuals[i]->genotype[p].pos;
					//population->individuals[i]->genotype[p] = (double) rand() / (double) RAND_MAX * 2 -1;
				}
		return population;
	}
	return NULL;
}

void printPopulation(Population *population)
{
	for(int i=0;i<population->individualsCnt;i++)
		printIndividual(population->individuals[i]);
}

void logPopulation(Population *population)
{
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	if(!pp)
		return;
	int indeks=population->individualsCnt-pp->gap.gaPopulationLogCnt;
	if(indeks<0)
		indeks = 0;
	for(int i=indeks;i<population->individualsCnt;i++)
		logIndividual(population->individuals[i],i);
  LOG(LOG_GA_POPULATION_AVG_FIT,"",population->avgFitt);
	LOG(LOG_GA_POPULATION_MIN_FIT,"",population->minFitt);
	LOG(LOG_GA_POPULATION_MAX_FIT,"",population->maxFitt);
}
