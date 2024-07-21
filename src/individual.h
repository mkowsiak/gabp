#ifndef _INDIVIDUAL_H
#define _INDIVIDUAL_H

#include "data.h"
#include "monitor.h"
#include "network.h"

#define NETWORK_BAD				0
#define NETWORK_GOOD			1
#define NETWORK_LEARNABLE	2
#define NETWORK_NOT_YET_DEFINED 3

struct GenotypePos
{
	double pos;
};

struct GenotypeInfo
{
	GenotypePos *cleanGenotype;
	char *stay;
	int width;
	int dataWidth;
	int in;
	int out;
};

struct Individual
{
	double fittnes;
	double nFitt;
	GenotypePos *genotype;
	int width;
	int dataWidth;
};

struct Phenotype
{
	Network *network;
	int aConGen;
	int conGen;
	int conMax;
	int learnCnt;
	int learnMax;
	int accTestCnt;
	int testCnt;
	int netConn;
	int category;
};

Phenotype *createPhenotype();
void			clearPhenotype(Phenotype *phenotype);
Phenotype *adapter(Individual *);

GenotypePos getGenotypePos(Individual *,int ,int );
GenotypePos getGenotypePos(GenotypePos *,int ,int ,int );
Individual *createIndividual(int );
Individual *cloneIndividual(Individual *);
void clearIndividual(Individual *);
double calculateIndividualFittnes(Individual *);
void printIndividual(Individual *);
void logIndividual(Individual *,int );
GenotypeInfo *createGenotypeInfo(Individual *);
void clearGenotypeInfo(GenotypeInfo *);
GenotypeInfo *cleanGenotype(Individual *);
double getFitnessValue(Phenotype *,double *);


#endif