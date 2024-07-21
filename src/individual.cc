#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "individual.h"
#include "monitor.h"
#include "memory.h"
#include "network.h"
#include "other.h"
#include "log.h"

/*
	The Magic Function. The Sense Of The Senses Of The Program.
	Use it with care, because it covers huge potential of force, and has very
	scientific name
	
	phenotype - is a phenotype
	factor - thous are multiply factors defined in parameters.dat file
	
	take a look at individual.h	
*/

double getFitnessValue(Phenotype *phenotype,double *factor)
{
	double fitness = 0.0000001;		// for being greater than 0
	if(phenotype->conGen)			
		fitness += factor[0] * ((double)phenotype->aConGen/(double)phenotype->conGen);
	if(phenotype->conGen)
		fitness += factor[1] * (1-(double)phenotype->aConGen/(double)phenotype->conGen);
	
	fitness += factor[2] * ((double)phenotype->conGen / (double)phenotype->conMax);
	fitness += factor[3] * (1-(double)phenotype->conGen / (double)phenotype->conMax);
	
	fitness += factor[4] * ((double)phenotype->learnCnt/(double)phenotype->learnMax);
	fitness += factor[5] * (1-(double)phenotype->learnCnt/(double)phenotype->learnMax);
	
	fitness += factor[6] * ((double)phenotype->accTestCnt/(double)phenotype->testCnt);
	fitness += factor[7] * (1-(double)phenotype->accTestCnt/(double)phenotype->testCnt);
	
	fitness += factor[8] * ((double)phenotype->aConGen);
	if(phenotype->aConGen)
		fitness += factor[9] * (1/(double)phenotype->aConGen);
	
	fitness += factor[10] * ((double)phenotype->conGen);
	if(phenotype->conGen)
		fitness += factor[11] * (1/(double)phenotype->conGen);
	
	fitness += factor[12] * ((double)phenotype->learnCnt);
	if(phenotype->learnCnt)
		fitness += factor[13] * (1/(double)phenotype->learnCnt);

	fitness += factor[14] * ((double)phenotype->accTestCnt);
	if(phenotype->accTestCnt)
		fitness += factor[15] *	(1/(double)phenotype->accTestCnt);
	
	fitness += factor[16] * ((double)phenotype->conMax);
	
	return fitness;
}

/*
	Returns single position of genotype from individual
*/

GenotypePos getGenotypePos(Individual *individual,int x,int y)
{
	return individual->genotype[x*individual->width+y-numberSequenceSum(x+1)];
}

/*
	Returns single position of genotype from genotype
*/

GenotypePos getGenotypePos(GenotypePos *genotype,int x,int y,int width)
{
	return genotype[x*width+y-numberSequenceSum(x+1)];
}

/*
	Set certain position of genotype
*/

void setGenotypePos(GenotypePos *genotype,int width,int x,int y,GenotypePos gpVal)
{
	genotype[x*width+y-numberSequenceSum(x+1)] = gpVal;
}

/*
	Traces genotype for inactive nodes and inactive connections
	connIn - defines wheter connections inside input are allowed
	connOut - as above but for output
*/

int traceGenotype(Individual *individual,GenotypeInfo *gInfo,int node,int connIn=0,int connOut=0)
{
	int marker = 0;
	/* If we are at the output node return 1. This mean that signal
		has been propagated succesfully */
	if(node >= (individual->width - gInfo->out))
	{
		gInfo->stay[node] = 1;	// mark node as avaible
		marker = 1;
	}
	/* For each nodes check if actual node is connected with other
			nodes. If does so, check if signal can be propagated*/
	for(int i=(node+1);i<individual->width;i++)
	{
		int isValid = 1;		// each nodes are valid, but ...
		/* if connections inside input nodes are not allowed
				and connection is inside input */
		if(!connIn)
			if(node < gInfo->in && i < gInfo->in)
				isValid = 0;	// it is forbidden
		/* if connections inside output nodes are not allowed
				and connection is inside output */
		if(!connOut)
			if(node >= gInfo->width - gInfo->out)
  			isValid = 0;	// it is forbidden
		/* Check the path from actual node in the output direction */
		if(getGenotypePos(individual,node,i).pos && isValid)
			if(traceGenotype(individual,gInfo,i,connIn,connOut))
			{
				gInfo->stay[node] = 1;	// if signal can be propagated, mark this node
				marker = 1;
			}
	}
	return marker;
}

/*
	Returns cleaned genotype. Cleaned means that there won't be
	invalid nodes and forbiden connections
*/

GenotypeInfo *cleanGenotype(Individual *individual)
{
	GenotypePos newPos;
	newPos.pos = 1;
	
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	if(!pp)
		return NULL;
	GenotypeInfo *gInfo = createGenotypeInfo(individual);
	assert(gInfo);
	if(!gInfo)
		return NULL;

	/* For each network input trace genotype */
	for(int i=0;i<pp->nnp.nnInputs;i++)
		traceGenotype(individual,gInfo,i,pp->nnp.nnConnectionsInsideInput,pp->nnp.nnConnectionsInsideOutput);
	/* for each position of connections matrix
		check if this position is valid */
	for(int i=0;i<individual->width;i++)
	{
		for(int p=i+1;p<individual->width;p++)
		{
			int isValid = 1;
			/* forbidden becouse of being inside input */
			if(!pp->nnp.nnConnectionsInsideInput)
				if(i < gInfo->in && p < gInfo->in)
					isValid = 0;
			/* forbidden becouse of being inside output*/
			if(pp->nnp.nnConnectionsInsideOutput)
				if(i >= gInfo->width - gInfo->out)
  				isValid = 0;
			/* if there is connection marked in connection matrix and both nodes
				are valid keep this connection inside genotype */
			if(getGenotypePos(individual,i,p).pos && gInfo->stay[p] && gInfo->stay[i] && isValid)
				setGenotypePos(gInfo->cleanGenotype,individual->width,i,p,newPos);
		}
	}
	return gInfo;
}

/*
	Prints some usefull informations about individual
*/

void printIndividual(Individual *individual)
{
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	if(!pp)
		return;
	if(pp->envp.envStdOutMode == 2)
		return;

	printf("addr: %x\n",individual);
	printf("width: %d\n",individual->width);
	printf("dataWidth: %d\n",individual->dataWidth);
	printf("fit:	%f\n",individual->fittnes);
	printf("genotype: ");
	for(int i=0;i<individual->dataWidth;i++)
		printf("%d",(unsigned int)individual->genotype[i].pos);
	printf("\n");
	GenotypeInfo *cleaned = cleanGenotype(individual);
	if(cleaned)
	{
		printf("genotypc: ");
		for(int i=0;i<individual->dataWidth;i++)
			printf("%d",(unsigned int)cleaned->cleanGenotype[i].pos);
		printf("\n");
		clearGenotypeInfo(cleaned);
	}
}

/*
	Logs individual.
*/
void logIndividual(Individual *individual,int no)
{
	char buf[1024];
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	if(!pp)
		return;
	if(pp->envp.envStdOutMode == 2)
		return;
  sprintf(buf,"Individual %d",no);	// Number of individual inside population
	LOG(LOG_GA_INDIVIDUAL_FIT,buf,individual->fittnes);
	for(int i=0;i<individual->dataWidth;i++)	// print each position of genotype
		sprintf(&buf[i],"%d",(unsigned int)individual->genotype[i].pos);
	buf[individual->dataWidth] = '\0';
	LOG(LOG_GA_INDIVIDUAL_GENOTYPE_O,buf);		// it is oryginal genotype
	GenotypeInfo *cleaned = cleanGenotype(individual);
	if(cleaned)
	{
		for(int i=0;i<individual->dataWidth;i++)
			sprintf(&buf[i],"%d",(unsigned int)cleaned->cleanGenotype[i].pos);
		buf[individual->dataWidth] = '\0';
		LOG(LOG_GA_INDIVIDUAL_GENOTYPE_C,buf);	// this one is cleaned
		clearGenotypeInfo(cleaned);							// always keep your place clean
	}
}

/*
	Individual creation ...
*/

Individual *createIndividual(int width)
{
	Individual *individual = (Individual*) _malloc(sizeof(Individual));
	assert(individual!=NULL);
	if(individual != NULL)
	{
		individual->fittnes = 0;
		individual->width = width;
		/* remember the width of genotype isn't quite equal with dataWidth
			Having 5 neurons in the net gives you 10 connections */
		individual->dataWidth=(width*width) - numberSequenceSum(width);
		individual->genotype = (GenotypePos*)_malloc(sizeof(GenotypePos)*individual->dataWidth);
		assert(individual->genotype != NULL);
		if(individual->genotype)
		{
			for(int i=0;i<individual->dataWidth;i++)
				// each position is random value
				individual->genotype[i].pos =  rand()%2 ;
		}
  	else
		{
			_free((void*)individual);
			individual = NULL;
			return NULL;
		}
		return individual;
	}
	else
		return NULL;
}


void clearIndividual(Individual *individual)
{
	if(individual)
	{
		_free((void*)individual->genotype);
		_free((void*)individual);
	}
}

GenotypeInfo *createGenotypeInfo(Individual *individual)
{
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	if(!pp)
		return NULL;
	GenotypeInfo *_new = (GenotypeInfo*)_malloc(sizeof(GenotypeInfo));
	assert(_new);
	if(!_new)
		return NULL;	
	_new->stay = (char*)_malloc(sizeof(char)*individual->width);
	_new->cleanGenotype = (GenotypePos*)_malloc(sizeof(GenotypePos)*individual->dataWidth);
	assert(_new->stay);
	assert(_new->cleanGenotype);
	if(!_new->stay || !_new->cleanGenotype)
	{
		clearGenotypeInfo(_new);
		return NULL;
	}		
	memset(_new->stay,0,individual->width);
	memset(_new->cleanGenotype,0,sizeof(GenotypePos)*individual->dataWidth);
	_new->width = individual->width;
	_new->dataWidth = individual->dataWidth;
	_new->in = pp->nnp.nnInputs;
	_new->out = pp->nnp.nnOutputs;
	return _new;
}

void clearGenotypeInfo(GenotypeInfo *gInfo)
{
	if(gInfo)
	{
		_free((void*)gInfo->cleanGenotype);
		_free((void*)gInfo->stay);
		_free((void*)gInfo);
		gInfo = NULL;
	}
}

double calculateIndividualFittnes(Individual *individual)
{
	Phenotype *phenotype;
	double error;
	double *outputError;
	double *Error;
	double *out;
	int		 *randPos;
	int accTest = 0;
	
	int znacznik = 0;
	int counter = 0;
	
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	if(!pp)
		return 0;

	phenotype = adapter(individual);
	assert(phenotype);
	if(!phenotype)
		return 0;

	if(!phenotype->category)
	{
		individual->fittnes =  getFitnessValue(phenotype,pp->nnp.nnFitnessParametersBad);
		clearPhenotype(phenotype);
		return individual->fittnes;
	}
	
	outputError = (double*) _malloc(sizeof(double)*pp->ds->out);
	Error = (double*) _malloc(sizeof(double)*pp->ds->dataCnt);
	randPos = (int*)	_malloc(sizeof(int)*pp->ds->dataCnt);
  assert(outputError);
  assert(Error);
  assert(randPos);
  if(!outputError || !Error)
  {
  	clearPhenotype(phenotype);
  	_free(outputError);
  	_free(Error);
  	_free(randPos);
  }
  for(int i=0;i<pp->ds->dataCnt;i++)
  	randPos[i] = i;
		
	if(phenotype->netConn)
	{
		do
		{
			shuffle(randPos,pp->ds->dataCnt);
			for(int i=0;i<pp->ds->dataCnt;i++)
			{
				int patternNo;
				if(pp->nnp.nnRandomPattern)
					patternNo = randPos[i];
				else
					patternNo = i;
				Error[i] = 0;	
				error = 0;
				double *dataOutput = getLearnOutput(pp->ds,patternNo);
				out = propagateSignal(phenotype->network,getLearnInput(pp->ds,patternNo));
				for(int p=0;p<pp->ds->out;p++)
				{
					outputError[p] = dataOutput[p] - out[p];
					error += outputError[p]*outputError[p];
				}
				error /= (double) pp->ds->out;
				Error[i] = error;
				Error[i] = sqrt(abs(Error[i]));
				propagateError(phenotype->network,outputError);
				modifyWeights(phenotype->network,pp->nnp.nnLearnRate,pp->nnp.nnMomentumRate);
			}
			znacznik = 0;	
			for(int i=0;i<pp->ds->dataCnt;i++)
			{
				if(Error[i] > pp->nnp.nnMaximalError)
					znacznik = 1;
			}		
			counter ++;
		}while(counter < pp->nnp.nnDataMaxPresentationsCnt && znacznik);

		double errorTest;
		for(int i=0;i<pp->ds->testCnt;i++)
		{
			out = propagateSignal(phenotype->network,getTestInput(pp->ds,i));
			errorTest = 0;
			for(int p=0;p<pp->ds->out;p++)
			{
				outputError[p] = getTestOutput(pp->ds,i)[p] - out[p];
				errorTest += outputError[p]*outputError[p];
			}
			errorTest /= (double) pp->ds->out;
			errorTest = sqrt(abs(errorTest));
			if(errorTest < pp->nnp.nnMaximalError)
				accTest ++;
		}
		
		phenotype->accTestCnt = accTest;
		phenotype->learnCnt		= counter;
					
		if(counter < pp->nnp.nnDataMaxPresentationsCnt)
			individual->fittnes = getFitnessValue(phenotype,pp->nnp.nnFitnessParametersLearned);
		else
			individual->fittnes = getFitnessValue(phenotype,pp->nnp.nnFitnessParametersGood);
		
		/* Echo everything about individual */
		if(pp->envp.envShowLearningProcess)
		{
			printf("\n");
			for(int i=0;i<pp->ds->dataCnt;i++)
			{
				error = 0;
				out = propagateSignal(phenotype->network,getLearnInput(pp->ds,i));
				for(int p=0;p<pp->ds->in;p++)
					printf("%.3f ",getLearnInput(pp->ds,i)[p]);
				printf(" -> ");
				for(int p=0;p<pp->ds->out;p++)
					printf(" %.3f ",out[p]);
				printf(" [");
				for(int p=0;p<pp->ds->out;p++)
					printf("%.3f ",getLearnOutput(pp->ds,i)[p]);
				printf("]\n");
			}
			printf("\naConGen: %d\n",phenotype->aConGen);
			printf("conGen: %d\n",phenotype->conGen);
			printf("conMax: %d\n",phenotype->conMax);
			printf("learnCnt: %d\n",phenotype->learnCnt);
			printf("learnMax: %d\n",phenotype->learnMax);
			printf("accTestCnt: %d\n",phenotype->accTestCnt);
			printf("testCnt: %d\n",phenotype->testCnt);
			printf("fitness: %f\n",individual->fittnes);
		}
	}
	else
		return 0;
 	clearPhenotype(phenotype);
	_free((void*)outputError);
	_free((void*)Error);
 	_free(randPos);	
	return individual->fittnes;
}

Individual *cloneIndividual(Individual *individual)
{
	assert(individual != NULL);
	if(!individual)
		return NULL;
	Individual *_new = createIndividual(individual->width);
	assert(_new != NULL);
	if(individual && _new)
	{
		_new -> dataWidth = individual->dataWidth;
		_new -> fittnes = individual->fittnes;
		_new->nFitt = individual->nFitt;
		for(int i=0;i<_new->dataWidth;i++)
			_new->genotype[i] = individual->genotype[i];
		return _new;
	}
	return NULL;
}
