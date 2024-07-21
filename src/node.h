#ifndef _NODE_H
#define _NODE_H

#include "activation_functions.h"

#define outputNodeNo(no) 	-((no+1)*2)
#define outputNode(no)		-(no/2)-1
#define inputNodeNo(no)		-(no*2)-1
#define inputNode(no)     -(no/2)

struct Node;

struct NodeConnection
{
	Node *node;
	int targetNo;
};

struct Node
{
	int 		ID;            							// ID of n-th node
	double 	*weights;										// inputs weights
	double  *dweights;									// value of weight change
	double 	net;												// net value of each node
	double 	error;											// error of each node
	double	delta;											// this is the value of derivative
	double	tdelta;											// this is last value of derivative
	double	wdelta;
	double	twdelta;										//
	double 	output;											// output of node =  f(net)
	int		 	forwardConnections;					// how many forward connection there is
	int			backwardConnections;				// how many backward connection there is
	ActivationFunction *activationF;		// activation function
	NodeConnection **forwardNodes;							// forward nodes
	NodeConnection **backwardNodes;						// backward nodes
	int			*forwardNodesNo;						// backward connection number
	int			*backwardNodesNo;						// forward connection number
};

Node *createNode(int,int =0,double *fP=NULL,double *dp=NULL );
Node *setWeights(Node *,double *);
Node *setWeights(Node *,double ,double);
void clearNode(Node *);
int addBackwardConnection(Node *,Node *);
int addForwardConnection(Node *,Node *);
int getNodeNoFast(Node *,int);
int getNodeNo(Node *,Node *);
void printNode(Node *);

#endif