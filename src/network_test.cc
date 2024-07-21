#include <stdio.h>
#include <math.h>
#include <assert.h>


#include "individual.h"
#include "monitor.h"
#include "memory.h"
#include "network.h"
#include "other.h"
#include "log.h"
#include "network_test.h"

double testNetwork(Individual *individual)
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
				out = propagateSignal(phenotype->network,getLearnInput(pp->ds,patternNo));
				for(int p=0;p<pp->ds->out;p++)
				{
					outputError[p] = getLearnOutput(pp->ds,patternNo)[p] - out[p];
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
		printf("\n");
		for(int i=0;i<pp->ds->dataCnt;i++)
		{
			error = 0;
			out = propagateSignal(phenotype->network,getLearnInput(pp->ds,i));
			for(int p=0;p<pp->ds->out;p++)
			{
				outputError[p] = getLearnOutput(pp->ds,i)[p] - out[p];
				error += outputError[p]*outputError[p];
			}
			error /= (double) pp->ds->out;
			error = sqrt(error);
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
	else
	{
		clearPhenotype(phenotype);
		_free((void*)outputError);
		_free((void*)Error);
 		_free(randPos);	
 		return 0;
 	}
	return individual->fittnes;
}

void testNetworksIfAvaible()
{
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	if(!pp)
		return;
	char *testNetworkGenotype;
	Individual *testIndividual = createIndividual(pp->gap.gaGenotypeSize);
	testNetworkGenotype = (char*)_malloc(sizeof(char)*testIndividual->dataWidth+1);
	assert(testNetworkGenotype);
	assert(testIndividual);
	if(testNetworkGenotype && testIndividual)
	{
		while(fgets(testNetworkGenotype,testIndividual->dataWidth+1,stdin))
		{
			printf("\nTesting network:\n%s\n",testNetworkGenotype);
			for(int i=0;i<testIndividual->dataWidth;i++)
				testIndividual->genotype[i].pos = (double)(testNetworkGenotype[i]-'0');
			testNetwork(testIndividual);
		}
	}
	_free(testNetworkGenotype);
	clearIndividual(testIndividual);
}
