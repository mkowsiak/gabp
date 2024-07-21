#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "monitor.h"
#include "node.h"
#include "network.h"
#include "activation_functions.h"

/*

	Name:						setBias
	Description: 		Sets the bias value
	Arguments:	    Network *network	- pointer to Network structure
									double dVal		- new value of bias
	Returned Value:	old bias value
	
*/

double setBias(Network *network,double dVal)
{
	double rVal = 0;
	assert(network != NULL);
	if(network)
	{
		rVal = network->bias.output;
		network->bias.output = dVal;
	}	
	return rVal;
}

/*

	Name:						printNetworkStatus
	Description: 		prints out the network status
	Arguments:	    Network *network	- pointer to Network structure
	Returned Value:	nothing
	
*/



void printNetworkStatus(Network *network)
{
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	if(!pp)
		return;
	if(pp->envp.envStdOutMode == 2)
		return;
	assert(network != NULL);
	if(network)
	{
		for(int i=0;i<network->allocated;i++)
		{
			printNode(network->nodes[i]);
			printf("\n-----\n");
		}
	}
}

/*

	Name:						findNode
	Description: 		finds the i'th node from the network
	Arguments:	    Network *network	- pointer to Network structure
									int nodeNo	- node number
	Returned Value:	pointer to node if exist
									NULL - if there is no node with index nodeNo
	
*/


Node *findNode(Network *network,int nodeNo)
{
	Node *out = NULL;
	assert(network != NULL);
	if(network)
	{
		out = getNode(network,nodeNo);
		if(out && out -> ID == nodeNo)
			return out;
		int allocated = network->allocated;
		for(int i=0;i<allocated;i++)
		{
			if(network->nodes[i]->ID == nodeNo)
				return network->nodes[i];
		}	
	}
	return NULL;
}


/*

	Name:						getNode
	Description: 		returns the i'th node from the network
	Arguments:	    Network *network	- pointer to Network structure
									int nodeNo	- node number
	Returned Value:	pointer to node if exist
									NULL - if there is no node with index nodeNo
	
*/

Node *getNode(Network *network,int ID)
{
	Node *node = NULL;
	assert(network != NULL);
	if(ID > 0)
	{
		node = network->nodes[network->inputs+network->outputs+ID-1];
		if(node && node -> ID == ID)
			return node;
	}
	if(ID % 2)
	{
		node = network->nodes[inputNode(ID)];
		if(node && node -> ID == ID)
			return node ;
	}
	else
	{
		node = network->nodes[network->inputs+outputNode(ID)];
		if(node && node -> ID == ID)
			return node;
	}
	return NULL;
}


/*

	Name:						addConnection
	Description: 		adds connection betwen two nodes
	Arguments:	    Network *network	- pointer to Network structure
									int sourceNode	- source node number
									int targetNode	- target node number
	Returned Value:	pointer to Network
									NULL - connection can not be established
	
*/


Network *addConnection(Network *network,int sourceNode,int targetNode)
{
	Node *source = NULL;
	Node *target = NULL;
	int		sourceNo;
	int		targetNo;
	assert(network != NULL);
	if(network)
	{
		source = findNode(network,sourceNode);
		target = findNode(network,targetNode);
		if(source && target)
		{
			sourceNo = addForwardConnection(source,target);
			targetNo = addBackwardConnection(target,source);
			if(sourceNo != -1 && targetNo != -1)
			{
				source -> forwardNodes[sourceNo] -> targetNo = targetNo;
				target -> backwardNodes[targetNo] -> targetNo = sourceNo;
				return network;
			}
		}
	}
	return NULL;
}

Network *addDummyConnections(Network *network)
{
/*	if(network -> nodesCnt >= network->inputs && network -> nodesCnt >= network->outputs)
	{*/
		ProgramParameters *pp = getProgramParameters();
		assert(pp);
		if(!pp)
			return NULL;
	
		assert(network != NULL);
		
		int inputs = network->inputs;
		int outputs = network->outputs;
		int nodesCnt = network->nodesCnt;
		

		if(pp->nnp.nnSignalPropagation)
		{
			for(int i=0;i<network->inputs;i++)
			{
				for(int p=0;p<inputs && p<nodesCnt;p++)
				{
					if(!addConnection(network,inputNodeNo(i),p+1))
						return NULL;
				}
			}
		}
		else
		{
			for(int p=0;p<inputs && p<nodesCnt;p++)
			{
				if(!addConnection(network,inputNodeNo(p),p+1))
					return NULL;
			}
		}

		for(int i=0;i<outputs && i <nodesCnt;i++)
		{
			if(!addConnection(network,nodesCnt-outputs+i+1,outputNodeNo(i)))
				return NULL;		
			setWeights(findNode(network,outputNodeNo(i)),1,1);
		}
		for(int i=0;i<nodesCnt;i++)
		{
			if(addBackwardConnection(findNode(network,i+1),&(network->bias))==-1)
				return NULL;
		}
		return network;
/*	}
	else
		return NULL;*/
}

Node *addNode(Network *network,int activationFunction,double *fP,double *dP)
{
	Node *node = NULL;
	assert(network != NULL);
	if(network)
		node = createNode(network->nodesCnt+1,activationFunction,fP,dP);
	assert(node != NULL);
	if(node)
	{
		network -> nodes = (Node**) _realloc(network -> nodes,sizeof(Node*)*(network->allocated+1));
		assert(network -> nodes != NULL);
		if(network->nodes)
		{
			network -> nodes[network->allocated] = node;
			network -> allocated ++;
			network -> nodesCnt ++;
			return node;
		}
		else
			clearNode(node);
	}
	return NULL;
}

Node *addNode(Network *network,Node *node)
{
	assert(network != NULL);
	assert(node != NULL);
	if(network)
	{
		network -> nodes = (Node**) _realloc(network -> nodes,sizeof(Node*)*(network->allocated+1));
		assert(network -> nodes != NULL);
		if(network->nodes && node)
		{
			network -> nodes[network->allocated] = node;
			network -> allocated ++;
			return node;
		}
	}
	return NULL;	
}

void clearNetwork(Network *network)
{
	if(network)
	{
		int allocated = network -> allocated;
		if(network->allocated && network->nodes)
		{
			for(int i=0;i<allocated;i++)
			{
				clearNode(network->nodes[i]);
			}
			_free((void*)network->nodes);
	  }
  	_free((void*)network->output);
	  _free((void*)network);
	}	
}

Network *createNetwork(int in,int out)
{
	int failure = 0;
	Network *network = (Network*)_malloc(sizeof(Network));
	assert(network != NULL);
	if(network)
	{
		network -> nodes = NULL;
		network -> nodesCnt = 0;
		network -> allocated = 0;
		network -> inputs = in;
		network -> outputs = out;
		network -> output = (double*)_malloc(sizeof(double)*out);
		network	-> bias.ID = 0;
		assert(network->output != NULL);
		if(!network->output)
			failure = 1;
		for(int i=0;i<in;i++)
		{
			if(!addNode(network,createNode(inputNodeNo(i))))
				failure = 1;
		}
		for(int i=0;i<out;i++)
		{
			if(!addNode(network,createNode(outputNodeNo(i))))
				failure = 1;
		}
		if(failure)
			clearNetwork(network);
	}
	return network;
}

double *propagateSignal(Network *network,double *netInputs)
{
	Node *node;
	assert(network != NULL);

	int inputs = network->inputs;  	
	int outputs = network->outputs;
	int nodesCnt = network->nodesCnt;

	
	for(int i=0;i<inputs;i++)
	{
		node = findNode(network,inputNodeNo(i));
		assert(node != NULL);
		if(node)
		{
			node -> output = netInputs[i];
		}
		else
			return NULL;
	}
	for(int i=0;i<nodesCnt;i++)
	{
		node = findNode(network,i+1);
		assert(node != NULL);
		if(node)
		{
			node -> net = 0;
			for(int p=0;p<node->backwardConnections;p++)
			{
				node -> net += node -> weights[p] * node -> backwardNodes[p] -> node -> output;
			}
			node -> output = node -> activationF -> function(node -> activationF -> functionParameters,node -> net);
		}
		else
			return NULL;
	}
	for(int i=0;i<outputs;i++)
	{
		node = findNode(network,outputNodeNo(i));
		assert(node != NULL);
		if(node)
		{
			node -> output = node -> backwardNodes[0] -> node -> output;  // backwardNodes[i] -> output;
			network->output[i] = node -> output;
		}
		else
			return NULL;
	}
	return network->output;
}

Network *propagateError(Network *network,double *error)
{
	Node *node = NULL;
	assert(network != NULL);
	if(!network)
		return NULL;
	
	int outputs = network->outputs;
	int nodesCnt = network->nodesCnt;
		
		
	for(int i=0;i<outputs;i++)
	{
		node = findNode(network,outputNodeNo(i));
		assert(node != NULL);
		if(node)
		{
			node -> error = error[i];
			node -> delta = error[i]; //node -> activationF -> derivative(node -> activationF -> derivativeParameters,node -> net);
		}
		else
			return NULL;
	}
	for(int i=nodesCnt;i>0;i--)
	{
		node = findNode(network,i);
		assert(node != NULL);
		if(node)
		{
			node -> error = 0;
			for(int i=0;i<node->forwardConnections;i++)
			{
				/*if(node -> forwardNodes[i] -> node -> weights[getNodeNoFast(node,i)] < 0.01)
					node -> error += 0.01;
				else*/
					node -> error += node -> forwardNodes[i] -> node -> delta * node -> forwardNodes[i] -> node -> weights[getNodeNoFast(node,i)];
			}
			node -> delta = node -> error * node -> activationF -> derivative(node -> activationF -> derivativeParameters,node -> net);
		}	
		else
			return NULL;
	}
	return network;
}

Network *modifyWeights(Network *network,double learnRate,double momentum)
{
	ProgramParameters *pp = getProgramParameters();
	assert(pp);
	if(!pp)
		return NULL;
	Node *node = NULL;
	assert(network != NULL);
	if(!network)
		return NULL;
		
	int nodesCnt = network->nodesCnt;
	double qpp = pp->nnp.nnQuickPropParameter; // alias
		
	for(int i=0;i<nodesCnt;i++)
	{
		node = findNode(network,i+1);
		assert(node != NULL);
		if(node)
		{
			for(int p=0;p<node -> backwardConnections;p++)
			{	
				double wdelta = 0;
				double lastweight;
				switch(pp->nnp.nnWeightModificationMethod)
				{
					case WEIGHT_MODIFICATION_METHOD_MOMENTUM_2:
						node -> weights[p] = node-> weights[p] + 2*learnRate*node -> delta * node -> backwardNodes[p] -> node -> output;
						node -> weights[p] += momentum * (node -> tdelta);
						node -> dweights[p] = 2*learnRate*node -> delta * node -> backwardNodes[p] -> node -> output;
						node -> tdelta = node -> delta;
					break;
					case WEIGHT_MODIFICATION_METHOD_QUICKPROP:
						lastweight = node->weights[p] - node->dweights[p];
						if(node -> dweights[p] > 0)
						{
							if(node->delta > 0)
								wdelta = learnRate*node->delta*node -> backwardNodes[p] -> node -> output;
							if(1+qpp)
								if(node->delta > qpp/(1+qpp)*node->tdelta)
									wdelta += qpp * node -> dweights[p];
							else
								if(node->tdelta-node->delta)
									wdelta += (node->delta*node -> backwardNodes[p] -> node -> output*node->dweights[p])/(node->tdelta-node->delta);
						}
						if(node -> dweights[p] < 0)
						{
							if(node -> delta < 0)
								wdelta = learnRate*node->delta*node -> backwardNodes[p] -> node -> output;
							if(1+qpp)
								if(node -> delta < qpp/(1+qpp)*node -> tdelta)
									wdelta += qpp*node -> dweights[p];
							else	
								if(node->tdelta-node->delta)
									wdelta += (node->delta*node -> backwardNodes[p] -> node -> output*node->dweights[p])/(node->tdelta-node->delta);
						}
						if(node->dweights[p] == 0)
							wdelta = learnRate * node -> delta*node -> backwardNodes[p] -> node -> output;						
						node -> weights[p] += wdelta;
					  node -> dweights[p] = wdelta;
					break;
					
					case WEIGHT_MODIFICATION_METHOD_MOMENTUM_1:
					default:
						node -> weights[p] = node-> weights[p] + 2*learnRate*node -> delta * node -> backwardNodes[p] -> node -> output;
						node -> weights[p] += momentum * (node -> weights[p] - node -> dweights[p]);
						node -> dweights[p] = node->weights[p];
					break;
				}
				node -> tdelta = node -> delta;
			}		
		}
		else
			return NULL;
	}
	return network;
}
