#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "data.h"
#include "node.h"
#include "network.h"
#include "population.h"
#include "individual.h"
#include "memory.h"
#include "activation_functions.h"
#include "network_test.h"
#include "sockets.h"
#include "monitor.h"
#include "other.h"
#include "parallel.h"
#include "log.h"
#include <mpi.h>

int main(int argCnt, char **args,char **env)
{
	int ID;							// ID of ther process
	int pCnt;						// processes count
		
	//char *logName = findEnvironmentString(env,"GABP_LOG_NAME=");
	char *sourceName = findEnvironmentString(env,"GABP_SRC_NAME=");
	
	char tmpName[256];

	extern Log *programLog;
	extern RegistryNode *programRegistry;
	extern ProgramParameters *programParameters;

	/* if we terminate program not all data will be lost*/
	
	signal(SIGABRT,GABP_abort);
	signal(SIGINT,GABP_abort);
	
	/* other signals ... not used, but there is a possibility */
	
	//signal(SIGKILL,GABP_abort);	
	//signal(SIGHUP,GABP_abort);
	signal(SIGQUIT,GABP_abort);
	/*signal(SIGTERM,GABP_abort);
	signal(SIGTTOU,GABP_abort);*/
		
	printf("\n  G(enetic)A(lgorithm)B(ack)P(ropagation) Copyright (C)2000-2001 Michal Owsiak\n");
	
	if(MPI_Init(&argCnt,&args) != MPI_SUCCESS)
	{
		printf(" !! MPI Initialization failed !!\n");
		return 0;
	}
	MPI_Comm_rank(MPI_COMM_WORLD,&ID);
	MPI_Comm_size(MPI_COMM_WORLD,&pCnt);
	
	srand((int)MPI_Wtime());
	//srand(0);
		
	programRegistry = createRegistryNode("empty");
	Population *population;

	if(sourceName)	
		sprintf(tmpName,"%s/defaults.dat",sourceName);
	else
		sprintf(tmpName,"/tmp/defaults.dat");		
		
	if(readDefaultProgramParameters(tmpName) == -1)
		printf("pID=%02d File with default parameters can not be found. Program may be unstable\n",ID);
  if(sourceName)
	  sprintf(tmpName,"%s/parameters.dat",sourceName);
	else
		sprintf(tmpName,"/tmp/parameters.dat");	 	
  int file = open(tmpName,O_RDONLY);
	if(file != -1)
	{
		programParameters=readProgramParameters(file,ID,pCnt);
		close(file);
	}
	else
	{
		printf("pID=%02d You have given wrong file name\n",ID);
		GABP_Abort(2);
	}
	
	if(!programParameters)
	{
		printf("\npID=%02d Quiting ! Program Parameters reading error !\n",ID);
		GABP_Abort(3);
	}
	
	char buf[256];
	char hostName[256];
	int pid = getpid();
	gethostname(hostName,256);
	sprintf(buf,"%s/%s%02d.%s.%09d",programParameters->envp.envOutputDir,
													programParameters->envp.envFileBaseName,
													ID,hostName,pid);
 	programLog = createLog(programParameters->envp.envLogSize,buf);
 	if(!programLog)
 	{
		printf("\npID=%02d ERROR ! Program LOG will not be created !!!\n",ID);
		fprintf(stderr,"pID=%02d ERROR ! Program LOG will not be created !!!\n",ID);
	}
	
	if(regInt("ReadError"))
	{
		if(programParameters->envp.envForce)
			printf("\nThere were some errors, but i'm forced to work anyway\n");			
		else
		{
			printf("\nThere were some errors. Correct them or use Force switch\n");
			clearProgramParameters(programParameters);
			clearRegistry(programRegistry);
			GABP_Abort(4);			
		}
	}

	
	if(programParameters->envp.envTestNetwork)
	{
		printf("\n!!! Program is going to work in test mode !!!\nUse stdin as input for genotype\n");
		testNetworksIfAvaible();
		clearProgramParameters(programParameters);
		clearRegistry(programRegistry);
		clearLog(programLog);
		MPI_Finalize();
		return 1;
	}
		
	population = createPopulation(  programParameters->gap.gaGenotypeSize,
																	programParameters->gap.gaMutation,
																	programParameters->gap.gaCrossOver);
  printf("my id=%d\n",ID);
	if(ID == 0)
	{
		int marker = 0;
		Individual **individuals = (Individual**)_malloc(sizeof(Individual*)*programParameters->gap.gaIndividualsCnt);
		for(int i=0;i<programParameters->gap.gaIndividualsCnt;i++)
		{
			individuals[i] = createIndividual(programParameters->gap.gaGenotypeSize);
			if(!individuals[i])
				marker = 1;			
		}
		if(marker)
		{
			for(int i=0;i<programParameters->gap.gaIndividualsCnt;i++)
				clearIndividual(individuals[i]);
			_free((void*)individuals);
			GABP_Abort(5);
		}
		for(int i=0;i<programParameters->gap.gaIndividualsCnt;i++)
		{
			int recv = i % pCnt;
			if(recv == 0)
			{
				Individual *_new = cloneIndividual(individuals[i]);
				addIndividual(population,_new);
			}
			else
			{
				sendIndividual(recv,individuals[i]);
			}
		}
		for(int i=0;i<programParameters->gap.gaIndividualsCnt;i++)
				clearIndividual(individuals[i]);
		_free((void*)individuals);
	}
	else
	{
		for(int i=0;i<programParameters->gap.gaIndividualsCnt/pCnt;i++)
		{
			Individual *_new = recvIndividual(0);
			addIndividual(population,_new);
		}
			
	}

	
	Population *tmp = NULL;

	for(int i=0;i<programParameters->gap.gaEpochsCnt;i++)
	{
		char buf[120];
		printf("id=%02d population %03d\n",ID,i);
		sprintf(buf,"fitnes populacji %03d",i);
		LOG(LOG_COMP_START,buf);
		double fit = calculatePopulationFittnes(population);
		if(fit==0)
		{
			printf("Blad obliczen !!!\n");
			GABP_Abort(6);
		}
		LOG(LOG_COMP_END,"koniec fitnes");
		
		
		if(!(i % programParameters->gap.gaCommunicationEpoch) && pCnt != 1)
			if(!globalCommunication(population))
			{
				fprintf(stderr,"	!! error within communication process. quiting !!");
				LOG(LOG_ERROR_COMMUNICATION_ERROR,"global comm");
				GABP_Abort(7);
			}
				
		LOG(LOG_IDLE_START,"po komunikacji");
		MPI_Barrier(MPI_COMM_WORLD);
		LOG(LOG_IDLE_END,"po komunikacji");

		LOG(LOG_COMP_START,"log start");
		logPopulation(population);
		LOG(LOG_COMP_END,"log koniec");
			
		LOG(LOG_COMP_START,"klonowanie start");
		tmp = clonePopulation(population);
		if(!tmp)
		{
			printf("Blad klonowania\n");
			GABP_Abort(8);
		}
		LOG(LOG_COMP_END,"klonowanie koniec");		
		
		LOG(LOG_COMP_START,"mutacja poczatek");
		mutatePopulation(tmp);
		LOG(LOG_COMP_END,"mutacja koniec");
		
		LOG(LOG_COMP_START,"clear poczatek");
		clearPopulation(population);
		LOG(LOG_COMP_END,"clear koniec");
		
		population = tmp;
	}
	
		
	LOG(LOG_IDLE_START,"bariera poczatek");
	MPI_Barrier(MPI_COMM_WORLD);
	LOG(LOG_IDLE_END,"bariera koniec");


	clearPopulation(population);
	clearProgramParameters(programParameters);
	
	clearRegistry(programRegistry);
	
	LOG(0xffff,"END OF PROGRAM");
	clearLog(programLog);
	
	MPI_Finalize();
	return 1;
}
