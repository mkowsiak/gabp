#ifndef _PARALLEL_H
#define _PARALLEL_H


#define PARALLEL_ERROR_UNKNOWN			0x0000
#define PARALLEL_ERROR_PROCESS_DOWN	0x0001	// next value in socket is a process no
#define PARALLEL_ERROR_SHUT_DOWN		0x0002	// the whole system is going down
#define PARALLEL_INDIVIDUAL 0x0101					// data in pipe contains an Individual description
#define PARALLEL_UNDEFINED	0xffff

#include "individual.h"
#include "monitor.h"
#include "population.h"


int readProgramParameters();
int writeProgramParameters(int );
int sendIndividual(int,Individual* );
Individual *recvIndividual(int );
int globalCommunication(Population *);
void pID2xy(int ,int *,int *,int );
int xy2pID(int *,int ,int ,int );
int mod(int ,int );
void GABP_Abort(int );


#endif