#ifndef _NETWORK_H
#define _NETWORK_H

#include "node.h"
#include "activation_functions.h"

#define	WEIGHT_MODIFICATION_METHOD_MOMENTUM_1 0	
#define WEIGHT_MODIFICATION_METHOD_MOMENTUM_2	1
#define WEIGHT_MODIFICATION_METHOD_QUICKPROP	2

struct Network
{
	int 	inputs;
	int 	outputs;
	int 	allocated;
	int		nodesCnt;
	double *output;
	Node 	bias;
	Node 	**nodes;
};

Network *createNetwork(int ,int );
Node *addNode(Network *,int =0,double*fP=NULL,double*dP=NULL);
Node *addNode(Network *,Node *);
Network *addConnection(Network *,int ,int );
Network *addDummyConnections(Network *);
double *propagateSignal(Network *,double *);
Network *propagateError(Network *,double *);
Network *modifyWeights(Network *,double, double);
void clearNetwork(Network *);
Node *findNode(Network *,int);
Node *getNode(Network *,int );
double setBias(Network *,double );
void printNetworkStatus(Network *);

#endif