#include <assert.h>
#include "parallel.h"
#include "node.h"
#include "data.h"
#include "memory.h"
#include "monitor.h"
#include "population.h"
#include "log.h"
#include <mpi.h>

void GABP_Abort(int errorCode)
{
	clearLog(getProgramLog());	
	clearProgramParameters(getProgramParameters());
	clearRegistry();
	MPI_Abort(MPI_COMM_WORLD,errorCode);
}

void pID2xy(int pID,int *x,int *y,int meshWidth)
{
	*x = pID % meshWidth;
	*y = pID / meshWidth;
}

int xy2pID(int x,int y,int meshWidth)
{
	int pID = y*meshWidth + x;
	return pID;
}

int mod(int x,int width)
{
	if(x < 0)
		return width - 1;
	if(x == width)
		return 0;
	return x;
}

int readProgramParameters()
{
	return 0;
}

int writeProgramParameters(int ID)
{
	return 0;
}

int sendIndividual(int ID,Individual *individual)
{
	assert(individual != NULL);
	if(!individual)
		return 0;
	
	LOG(LOG_IDLE_START,"ind send init");
	MPI_Send(&(individual->width),1,MPI_INT,ID,PARALLEL_INDIVIDUAL,MPI_COMM_WORLD);
	LOG(LOG_IDLE_END,"ind send init end");	

	LOG(LOG_COMM_START,"int send transfer");
	MPI_Send(individual->genotype,individual->dataWidth*sizeof(GenotypePos),MPI_CHAR,ID,PARALLEL_INDIVIDUAL,MPI_COMM_WORLD);
	MPI_Send(&(individual->fittnes),1,MPI_DOUBLE,ID,PARALLEL_INDIVIDUAL,MPI_COMM_WORLD);
	MPI_Send(&(individual->nFitt),1,MPI_DOUBLE,ID,PARALLEL_INDIVIDUAL,MPI_COMM_WORLD);
	LOG(LOG_COMM_END,"int send accom");
			
	return 1;
}

Individual *recvIndividual(int ID)
{
	MPI_Status  status;
	int width;
	
	Individual *_new;
	LOG(LOG_IDLE_START,"ind await");
	MPI_Recv(&width,1,MPI_INT,ID,PARALLEL_INDIVIDUAL,MPI_COMM_WORLD,&status);
	LOG(LOG_IDLE_END,"ind is co2ming");
	
	_new = createIndividual(width);
	assert(_new != NULL);
	if(!_new)
		return NULL;
		
	LOG(LOG_COMM_START,"ind arrive");
	MPI_Recv(_new->genotype,_new->dataWidth*sizeof(GenotypePos),MPI_CHAR,ID,PARALLEL_INDIVIDUAL,MPI_COMM_WORLD,&status);
	MPI_Recv(&(_new->fittnes),1,MPI_DOUBLE,ID,PARALLEL_INDIVIDUAL,MPI_COMM_WORLD,&status);
	MPI_Recv(&(_new->nFitt),1,MPI_DOUBLE,ID,PARALLEL_INDIVIDUAL,MPI_COMM_WORLD,&status);
	LOG(LOG_COMM_END,"ind is here");
	return _new;
}

int globalCommunication(Population *population)
{

	int x,y;
	int pID,pCnt;
	int meshWidth;
	int meshHeight;
	int toSendCnt;
	int toSend;
	int dstID;
	ProgramParameters *pp = getProgramParameters();
	Individual *individualToAdapt;
	assert(pp);
	if(!pp)
		return 0;
	population->lastAdapted = 0;	
	pID = pp->parp.parProcessID;
	pCnt = pp->parp.parProcessCnt;
	meshWidth = pp->parp.parMeshWidth;
	meshHeight = pp->parp.parMeshHeight;
	printf("pId = %d %f %d\n",pID,pp->gap.gaTransferParameter,population->individualsCnt);
	toSendCnt =  (int)(pp->gap.gaTransferParameter*(double)population->individualsCnt);
	if(!toSendCnt)
	{
		toSendCnt = 1;
		/*fprintf(stderr,"!! error int transfer parameter !!\n");
		LOG(LOG_ERROR_COMMUNICATION_ERROR,"error int transfer parameter");
		return 0;*/
	
	}
	toSend = population->individualsCnt - 1;
	pID2xy(pID,&x,&y,meshWidth);

	
	/* rozeslanie w rzedach 1 */	

	if(toSendCnt)
	{
		if(x%2 == 0)
		{
			dstID = xy2pID(mod(x-1,meshWidth),y,meshWidth);
			sendIndividual(dstID,population->individuals[toSend]);
			individualToAdapt = recvIndividual(dstID);
		}
		else
		{
			dstID = xy2pID(mod(x+1,meshWidth),y,meshWidth);
			individualToAdapt = recvIndividual(dstID);
			sendIndividual(dstID,population->individuals[toSend]);
		}
	
		assert(individualToAdapt);
		if(!individualToAdapt)
		{
			LOG(LOG_ERROR_COMMUNICATION_ERROR,"there was no individual");
		}
		else
		{
			LOG(LOG_COMP_START,"ind adapt");
			adaptIndividual(population,individualToAdapt);
			LOG(LOG_COMP_END,"ind adapted");
		}
		toSendCnt -= 1;
		toSend -= 1;
		LOG(LOG_IDLE_START,"bariera");
		MPI_Barrier(MPI_COMM_WORLD);
		LOG(LOG_IDLE_END,"bariera");
	}
		
		/* rozeslanie w rzedach 2 */

	if(toSendCnt)
	{					
		if(x%2 == 0)
		{
			dstID = xy2pID(mod(x+1,meshWidth),y,meshWidth);
			sendIndividual(dstID,population->individuals[toSend]);
			individualToAdapt = recvIndividual(dstID);
		}
		else
		{
			dstID = xy2pID(mod(x-1,meshWidth),y,meshWidth);
			individualToAdapt = recvIndividual(dstID);
			sendIndividual(dstID,population->individuals[toSend]);
		}
	
		assert(individualToAdapt);
		if(!individualToAdapt)
		{
			LOG(LOG_ERROR_COMMUNICATION_ERROR,"there was no individual");
		}
		else
		{
			LOG(LOG_COMP_START,"ind adapt");
			adaptIndividual(population,individualToAdapt);
			LOG(LOG_COMP_END,"ind adapted");
		}
		toSendCnt -= 1;
		toSend -= 1;
		LOG(LOG_IDLE_START,"bariera");
		MPI_Barrier(MPI_COMM_WORLD);
		LOG(LOG_IDLE_END,"bariera");
	}

		
		/* rozeslanie w kolumnach 1 */			

	if(toSendCnt)
	{		
		if(y%2 == 0)
		{
			dstID = xy2pID(x,mod(y-1,meshHeight),meshWidth);
			sendIndividual(dstID,population->individuals[toSend]);
			individualToAdapt = recvIndividual(dstID);
		}
		else
		{
			dstID = xy2pID(x,mod(y+1,meshHeight),meshWidth);
			individualToAdapt = recvIndividual(dstID);
			sendIndividual(dstID,population->individuals[toSend]);
		}
	
		assert(individualToAdapt);
		if(!individualToAdapt)
		{
			LOG(LOG_ERROR_COMMUNICATION_ERROR,"there was no individual");
		}
		else
		{
			LOG(LOG_COMP_START,"ind adapt");
			adaptIndividual(population,individualToAdapt);
			LOG(LOG_COMP_END,"ind adapted");
		}
		toSendCnt -= 1;
		toSend -= 1;
		LOG(LOG_IDLE_START,"bariera");
		MPI_Barrier(MPI_COMM_WORLD);
		LOG(LOG_IDLE_END,"bariera");
	}
			

    /* rozeslanie w kolumnach 2 */

	if(toSendCnt)
	{						
		if(y%2 == 0)
		{
			dstID = xy2pID(x,mod(y+1,meshHeight),meshWidth);
			sendIndividual(dstID,population->individuals[toSend]);
			individualToAdapt = recvIndividual(dstID);
		}
		else
		{
			dstID = xy2pID(x,mod(y-1,meshHeight),meshWidth);
			individualToAdapt = recvIndividual(dstID);
			sendIndividual(dstID,population->individuals[toSend]);
		}
	
		assert(individualToAdapt);
		if(!individualToAdapt)
		{
			LOG(LOG_ERROR_COMMUNICATION_ERROR,"there was no individual");
		}
		else
		{
			LOG(LOG_COMP_START,"ind adapt");
			adaptIndividual(population,individualToAdapt);
			LOG(LOG_COMP_END,"ind adapted");
		}
		toSendCnt -= 1;
		toSend-=1;
		LOG(LOG_IDLE_START,"bariera");
		MPI_Barrier(MPI_COMM_WORLD);
		LOG(LOG_IDLE_END,"bariera");
	}
		
	return 1;
}
