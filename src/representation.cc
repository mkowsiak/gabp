#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "individual.h"
#include "network.h"
#include "memory.h"


Phenotype *adapter(Individual *individual)
{
	Phenotype *phenotype;
	int activeConnections = 0;
	int allConnections 		= 0;
	
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	if(!pp)
		return NULL;
	
	phenotype = createPhenotype();
	assert(phenotype);
	if(!phenotype)
		return NULL;
		
  GenotypeInfo *gInfo = cleanGenotype(individual);
	assert(gInfo);
	if(!gInfo)
	{
		clearPhenotype(phenotype);
		return NULL;
	}
	for(int i=0;i<gInfo->dataWidth;i++)
	{
		if(individual->genotype[i].pos)
			allConnections ++;
		if(gInfo->cleanGenotype[i].pos)
			activeConnections++;
	}
	
	phenotype->aConGen 	= activeConnections;
	phenotype->conGen 	= allConnections;
	phenotype->conMax 	= individual->dataWidth;
	phenotype->learnMax = pp->nnp.nnDataMaxPresentationsCnt;
	phenotype->testCnt 	= pp->ds->testCnt;

	for(int i=0;i<gInfo->out;i++)
		if(gInfo->stay[gInfo->width-1-i] == 0)
		{
			clearGenotypeInfo(gInfo);
			phenotype->category = NETWORK_BAD;
			return phenotype;
		}

	phenotype->network = createNetwork(pp->ds->in,pp->ds->out);
	
	assert(phenotype->network!=NULL);
	if(!phenotype->network)
	{
		clearGenotypeInfo(gInfo);
		clearPhenotype(phenotype);
		return NULL;
	}
	for(int i=0;i<individual->width;i++)
	{
		if(!addNode(phenotype->network,pp->nnp.nnACFMode,pp->nnp.nnAFP,pp->nnp.nnADP))
		{
			clearPhenotype(phenotype);
			clearGenotypeInfo(gInfo);
			return NULL;
		}
	}
	for(int i=0;i<individual->width;i++)
	{
		for(int p=i+1;p<individual->width;p++)
			if(getGenotypePos(gInfo->cleanGenotype,i,p,individual->width).pos)
			{
				if(!addConnection(phenotype->network,i+1,p+1))
				{
					clearPhenotype(phenotype);
					clearGenotypeInfo(gInfo);
					return NULL;
				}
				phenotype->netConn ++;
			}
	}
	if(!addDummyConnections(phenotype->network))
	{
		clearPhenotype(phenotype);
		clearGenotypeInfo(gInfo);
		return NULL;
	}
	setBias(phenotype->network,pp->nnp.nnBiasMode);
	phenotype->category = NETWORK_NOT_YET_DEFINED;
	clearGenotypeInfo(gInfo);
	return phenotype;
}

Phenotype *createPhenotype()
{
	Phenotype *_new = (Phenotype*)malloc(sizeof(Phenotype));
	assert(_new != NULL);
	if(_new)
		memset(_new,0,sizeof(Phenotype));
	return _new;
}

void clearPhenotype(Phenotype *phenotype)
{
	if(phenotype)
	{
		clearNetwork(phenotype->network);
		_free(phenotype);
	}
}