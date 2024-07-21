#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "monitor.h"
#include "node.h"
#include "network.h"
#include "activation_functions.h"

void printNode(Node *node)
{
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	if(!pp)
		return;
	if(pp->envp.envStdOutMode == 2)
		return;
	assert(node != NULL);
	if(node)
	{
		printf("Node ID: %02d addres: %x\n",node -> ID,&node);
		printf("net = %f, error = %f, delta = %f, output = %f\n",node->net,node->error,node->delta,node->output);
		printf("forward: %02d\n",node -> forwardConnections);
		for(int i=0;i<node -> forwardConnections;i++)
		{
			printf("\"%x,%d\" ",&(node->forwardNodes[i]->node),node->forwardNodes[i]->node->ID);
		}
		printf("\nbackward: %02d\n",node -> backwardConnections);
		for(int i=0;i<node->backwardConnections;i++)
		{
			printf("\"%x,%f,%d\" ",&(node->backwardNodes[i]->node),node->weights[i],node->backwardNodes[i]->node->ID);
		}
	}
}

int getNodeNoFast(Node *node,int targetNo)
{
	assert(node != NULL);
	if(!node || targetNo < 0)
		return -1;
	return node -> forwardNodes[targetNo] -> targetNo;
}

int getNodeNo(Node *node,Node *target)
{
	assert(node != NULL);
	assert(target != NULL);
	if(node && target)
	{
		for(int i=0;i<node->backwardConnections;i++)
		{
			if(node->backwardNodes[i]->node == target)
				return i;
		}	
	}
	return -1;
}

Node *createNode(int ID,int activationFunction,double *fP,double *dP)
{
	Node *node = (Node*)_malloc(sizeof(Node));
	assert(node != NULL);
	if(node)
	{
		node->ID=ID;
		node->weights = NULL;
		node->dweights = NULL;
		node->activationF = createActivationFunction(activationFunction,fP,dP);
		node->net = 0;
		node->error = 0;
		node->output = 0;
		node->delta = 0;
		node->tdelta = 0;
		node->wdelta=0;
		node->twdelta =0;
		node->forwardConnections = 0;
		node->backwardConnections = 0;
		node->forwardNodes = NULL;
		node->backwardNodes = NULL;
		node->forwardNodesNo = NULL;
		node->backwardNodesNo = NULL;
		
		assert(node->activationF != NULL);
		
		if(!node->activationF)
			_free((void*)node);
	}
	return node;
}

Node *setWeights(Node *node,double *weights)
{
	assert(node != NULL);
	assert(weights != NULL);
	if(node && weights)
	{
		for(int i=0;i<node->backwardConnections;i++)
		{
			node->weights[i] = weights[i];
		}
		return node;
	}
	return NULL;
}

Node *setWeights(Node *node,double left=-1,double right=-1)
{
	assert(node != NULL);
	if(node)
	{
		for(int i=0;i<node->backwardConnections;i++)
		{
			node -> weights[i] = rand()/(double)RAND_MAX*(right-left)+left;
		}
		return node;
	}
	return NULL;
}

Node *setWeight(Node *node,int weightNo,double dVal)
{
	assert(node != NULL);
	if(node)
	{
		if(weightNo < node -> backwardConnections)
			node -> weights[weightNo]  = dVal;
		else
			return NULL;
	}	
	return node;
}

int addForwardConnection(Node *source,Node *target)
{
	assert(source != NULL);
	assert(target != NULL);
	if(source && target)
	{
		source -> forwardNodes = (NodeConnection **) _realloc(source -> forwardNodes,sizeof(NodeConnection*)*(source->forwardConnections + 1));
		assert(source -> forwardNodes != NULL);
		if(source -> forwardNodes)
		{
			source -> forwardNodes[source -> forwardConnections] = (NodeConnection*)_malloc(sizeof(NodeConnection));
			assert(source -> forwardNodes[source -> forwardConnections] != NULL);
			if(!source -> forwardNodes[source -> forwardConnections])
				return -1;
			source -> forwardNodes[source -> forwardConnections] -> targetNo = -1;
			source -> forwardNodes[source -> forwardConnections] -> node = target;
			source -> forwardConnections ++;
			return source -> forwardConnections - 1;
		}
	}
	return -1;
}

int addBackwardConnection(Node *source,Node *target)
{
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	assert(source != NULL);
	assert(target != NULL);
	if(source && target && pp)
	{
		source -> backwardNodes = (NodeConnection **) _realloc(source -> backwardNodes,sizeof(NodeConnection*)*(source->backwardConnections + 1));
		assert(source -> backwardNodes != NULL);
		if(source -> backwardNodes)
		{
			source -> backwardNodes[source -> backwardConnections] = (NodeConnection*)_malloc(sizeof(NodeConnection));
			assert(source -> backwardNodes[source -> backwardConnections] != NULL);
			if(!source -> backwardNodes[source -> backwardConnections])
				return -1;
			source -> backwardNodes[source -> backwardConnections] -> targetNo = 1;			
			source -> backwardNodes[source -> backwardConnections] -> node = target;
			source -> weights = (double*) _realloc(source -> weights,sizeof(double)*(source->backwardConnections+1));
			assert(source -> weights != NULL);
			source -> dweights = (double*) _realloc(source->dweights,sizeof(double)*(source->backwardConnections+1));
			assert(source -> dweights != NULL);
			
			source -> backwardConnections ++;
			if(source -> weights && source -> dweights)
			{
				source -> weights[source -> backwardConnections-1] = rand()/(double)RAND_MAX*(pp->nnp.nnRightWeight-pp->nnp.nnLeftWeight)+pp->nnp.nnLeftWeight;
				source -> dweights[source ->backwardConnections-1] = source -> weights[source -> backwardConnections-1];
				return source -> backwardConnections - 1;
			}
		}
	}
	return -1;
}

void clearNode(Node *node)
{
	if(node)
	{
		_free((void*)node->weights);
		_free((void*)node->dweights);
		if(node->activationF)
		{
			_free((void*)node->activationF->functionParameters);
			_free((void*)node->activationF->derivativeParameters);
			node->activationF->function = NULL;
			node->activationF->derivative = NULL;
			_free((void*)node->activationF);
		}
		for(int i=0;i<node->forwardConnections;i++)
			_free((void*)node->forwardNodes[i]);
		for(int i=0;i<node->backwardConnections;i++)
			_free((void*)node->backwardNodes[i]);
		_free((void*)node->forwardNodes);
		_free((void*)node->backwardNodes);
		_free((void*)node->forwardNodesNo);
		_free((void*)node->backwardNodesNo);
		_free((void*)node);
	}
}

