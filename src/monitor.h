#ifndef _MONITOR_H
#define _MONITOR_H

#include <stdio.h>
#include "data.h"
#include "registry.h"

/*
	Here are some freaky things. stdout is a makro.
	In linux it goes like this
		extern FILE *stdout;
		#define stdout stdout
	but in SunOs you will find something like this
	#define stdout &__iob[1] - iob is a FILE table
	If it is defined as above you won't be able to write
	stdout = ... becouse there is no l-value.
	Simply uncomment your architecture.... */
	
#define STDOUT_LINUX				
//#define STDOUT_HPPA
//#define STDOUT_SUN4SOL2

struct NNParameters
{
	double 	nnLearnRate;
	double 	nnMomentumRate;
	
	int			nnACFMode;						//   0-raw, 1-unipolar, 2-bipolar, 3-unisigmoidal, 4-bisigmoidal
		
	int 		nnAFPCnt;
	double 	*nnAFP;

	int			nnADPCnt;
	double	*nnADP;
	
	int 		nnDataMaxPresentationsCnt;
	double 	nnMaximalError;
	
	int 		nnInputs;
	int 		nnOutputs;
	
	double 	nnLeftWeight;
	double 	nnRightWeight;
	
	double	nnBiasMode;

	int 		nnLearnDataCnt;
	int			nnTestDataCnt;
	
	double  *nnFitnessParametersBad;
	int			nnFitnessParametersBadCnt;
	
	double 	*nnFitnessParametersGood;
	int			nnFitnessParametersGoodCnt;
	
	double  *nnFitnessParametersLearned;
	int     nnFitnessParametersLearnedCnt;
	
	int			nnConnectionsInsideInput;
	int			nnConnectionsInsideOutput;
	
	int			nnSignalPropagation;
	int			nnRandomPattern;
	
	int			nnWeightModificationMethod;
	
	double	nnQuickPropParameter;
		
	double 	nnActivationThreshold;
};

struct GAParameters
{
	int gaInitMode;
	double gaCrossOver;
	double gaMutation;
	int	gaNormalization;
	double gaLeftNorm;
	double gaRightNorm;
	int	gaFitnessByPosition;
	int gaEpochsCnt;
	int gaIndividualsCnt;
	int gaGenotypeSize;
	int gaPopulationLogCnt;
	double gaTransferParameter;
	int gaCommunicationEpoch;
	int gaFitCnt;
};

struct EnvParameters
{
	FILE *envStdOut;
	FILE *oldStdOut;
	char *envOutputDir;
	char *envFileBaseName;
	char *envLearnDataFileName;
	char *envTestDataFileName;
	char *envStdOutFileName;
	int		envStdOutMode;
	int		envLogSize;	
	int		envPrintfMode;
	int		envTestNetwork;
	int		envForce;
	int		envShowLearningProcess;
};

struct ParParameters
{
	int parMeshWidth;
	int parMeshHeight;
	int parProcessID;
	int parProcessCnt;
};

struct ProgramParameters
{
	DataSet 			*ds;
	NNParameters 	nnp;
	GAParameters 	gap;
	EnvParameters envp;	
	ParParameters parp;	
};



ProgramParameters *createProgramParameters(int,int);
void clearProgramParameters(ProgramParameters*);
ProgramParameters *readProgramParameters(int,int=0,int=1);
int readDefaultProgramParameters(char *);
ProgramParameters *getProgramParameters();

#endif