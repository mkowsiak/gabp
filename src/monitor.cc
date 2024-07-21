#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "data.h"
#include "monitor.h"
#include "memory.h"
#include "other.h"
#include "sockets.h"
#include <stdlib.h>

ProgramParameters *programParameters = NULL;

ProgramParameters *createProgramParameters(int in,int out)
{
	ProgramParameters *_new = (ProgramParameters*) _malloc(sizeof(ProgramParameters));
	memset(_new,0,sizeof(ProgramParameters));
	_new -> nnp.nnInputs = in;
	_new -> nnp.nnOutputs = out;
	assert(_new != NULL);
	if(!_new)
		return NULL;
	_new -> ds = createDataSet(in,out);
	assert(_new -> ds != NULL);
	if(_new -> ds)
	{
		return _new;
	}
	else
	{
		clearDataSet(_new -> ds );
		_free((void*)_new);
	}
	return NULL;	
}

void clearProgramParameters(ProgramParameters *pp)
{
	if(pp)
	{
		if(pp->ds)
			clearDataSet(pp->ds);
		if(pp->nnp.nnAFP)
			_free(pp->nnp.nnAFP);
		if(pp->nnp.nnADP)
			_free(pp->nnp.nnADP);
		if(pp->nnp.nnFitnessParametersBad)
			_free(pp->nnp.nnFitnessParametersBad);
		if(pp->nnp.nnFitnessParametersGood)
			_free(pp->nnp.nnFitnessParametersGood);
		if(pp->nnp.nnFitnessParametersLearned)
			_free(pp->nnp.nnFitnessParametersLearned);
		if(pp->envp.envOutputDir)
			_free(pp->envp.envOutputDir);
		if(pp->envp.envFileBaseName)
			_free(pp->envp.envFileBaseName);
		if(pp->envp.envLearnDataFileName)
			_free(pp->envp.envLearnDataFileName);
		if(pp->envp.envTestDataFileName);
			_free(pp->envp.envTestDataFileName);
		if(pp->envp.envStdOutFileName)
			_free(pp->envp.envStdOutFileName);
		if(pp->envp.envStdOutMode)
		{
			if(pp->envp.envStdOut)
			{
				fclose(pp->envp.envStdOut);
#if defined(STDOUT_LINUX)
				stdout = pp->envp.oldStdOut;
#endif
#if defined(STDOUT_HPPA)
	#if	defined(__STDC__)
				__iob[1] = *pp->envp.oldStdOut;
	#else
				_iob[1] = *pp->envp.oldStdOut;
	#endif
#endif

#ifdef STDOUT_SUN4SOL2
	#if defined(__STDC__)
				__iob[1] = *pp->envp.oldStdOut;
	#else
				_iob[1] = *pp->envp.oldStdOut;
	#endif
#endif				
			}
		}
		_free(pp);
	}
}

int lookForInt(char *node)
{
	int iVal;
	int i;
	char tmp[256] = "default";
	char string[256];
	sprintf(string,"Looking for \"%s\" ",node);
	for(i=strlen(string);i<50;i++)
		string[i] = '.';
	string[i] = '\0';
	printf("%s ",string);
	iVal = regInt(node);
	if(iVal != 0xffff)
		printf(" OK\n");
	else
	{
		printf("not found\n");
		strcat(tmp,node);		
		iVal = regInt(tmp);
		if(!findRegistryNode("ReadError"))
		{
			addRegistryNode("ReadError");
			addRegistryElement("ReadError","1");
		}
	}
	return iVal;
}

double lookForDouble(char *node)
{
	double dVal;
	int i;
	char tmp[256] = "default";
	char string[256];
	sprintf(string,"Looking for \"%s\" ",node);
	for(i=strlen(string);i<50;i++)
		string[i] = '.';
	string[i] = '\0';
	printf("%s ",string);
	dVal = regDouble(node);
	if(dVal != 0xffff)
		printf(" OK\n");
	else
	{
		printf("not found\n");
		strcat(tmp,node);		
		dVal = regDouble(tmp);
		if(!findRegistryNode("ReadError"))
		{
			addRegistryNode("ReadError");
			addRegistryElement("ReadError","1");
		}
	}
	return dVal;
}

char *lookForString(char *node)
{
	char *cVal;
	char tmp[256] = "default";
	char string[256];
	int i;
	sprintf(string,"Looking for \"%s\" ",node);
	for(i=strlen(string);i<50;i++)
		string[i] = '.';
	string[i] = '\0';
	printf("%s ",string);
	cVal = regString(node);
	if(cVal)
		printf(" OK\n");
	else
	{
		printf("not found\n");
		strcat(tmp,node);		
		cVal = regString(tmp);
		if(!findRegistryNode("ReadError"))
		{
			addRegistryNode("ReadError");
			addRegistryElement("ReadError","1");
		}
	}
	return cVal;
}

int readDefaultProgramParameters(char *fileName)
{
	char line[256];
	char **tokens;
	int file = open(fileName,O_RDONLY);
	if(file == -1)
	{
		printf("Oh, no, there is no default parameters file ...\n");
		return file;
	}
	while(readLine(file,line))
	{
		tokens = getTokens(line);
		int i = 0;
		if(tokens[i])
		{
			addRegistryNode(tokens[i]);
			i++;
		}
		while(tokens[i])
		{
			addRegistryElement(tokens[0],tokens[i]);
			i++;
		}
		clearTokens(tokens);
	}
	return file;
}

ProgramParameters *readProgramParameters(int file,int processID,int processCnt)
{
	ProgramParameters *pp;
	int tmpFile;
	char line[256];
	char tmpLine[256];
	char **tokens;
	double 	dVal;
	double	dVal2;
	int			iVal;
	if(file <= 0)
	{
		printf("Terrible error occured. Reading file failed\n");
		return NULL;
	}
	while(readLine(file,line))
	{
		tokens = getTokens(line);
		int i = 0;
		if(tokens[i])
		{
			addRegistryNode(tokens[i]);
			i++;
		}
		while(tokens[i])
		{
			addRegistryElement(tokens[0],tokens[i]);
			i++;
		}
		clearTokens(tokens);
	}
		
	int inputs = lookForInt("Inputs");	// setting up number of inputs and outputs
	int outputs = lookForInt("Outputs");
	if(inputs <= 0)
		inputs = regInt("defaultInputs");
	if(outputs <= 0)
		outputs = regInt("defaultOutputs");
	if(inputs <= 0 || outputs <= 0)
	{
		printf("	ERROR ! You haven't specified the number of inputs or outputs of the net\n");
		return NULL;
	}
	
	/* The Creation */
	pp = createProgramParameters(inputs,outputs);
	assert(pp != NULL);
	if(!pp)
		return NULL;	// and Destruction
	
	pp->parp.parProcessID = processID;			// set the number of process
	pp->parp.parProcessCnt = processCnt;	
	
	
	/* Reading of screen data output mode. Not working anyway
		There are some problems with stdout file handle in SunOS,
		but there is a possibility for this issue in Linux */
		
	if(regNodeSize("StdOut") >= processCnt)		// if there is no enought parameters on the list
		iVal = regListInt("StdOut",processID);
	else
		iVal = regInt("defaultStdOut");					// use standard stdout
	if(iVal != 0 && iVal != 1)
	{
		printf("  StdOut must be set either to 0, or 1. Setting to 0\n");
		pp->envp.envStdOutMode = 0;
	}
	else
		pp->envp.envStdOutMode = iVal;	
	
	/* Some usefull path names */
	pp->envp.envStdOutFileName		= _strdup(lookForString("StdOutFileName"));
	pp->envp.envOutputDir					= _strdup(lookForString("OutputDir"));
  pp->envp.envFileBaseName			= _strdup(lookForString("FileBaseName"));
	pp->envp.envLearnDataFileName	= _strdup(lookForString("LearnDataFileName"));
	pp->envp.envTestDataFileName	= _strdup(lookForString("TestDataFileName"));
	assert(pp->envp.envLearnDataFileName != NULL);
	assert(pp->envp.envTestDataFileName != NULL);
	assert(pp->envp.envOutputDir != NULL);
	assert(pp->envp.envFileBaseName != NULL);
	assert(pp->envp.envStdOutFileName);
	
	if(!pp->envp.envLearnDataFileName || !pp->envp.envTestDataFileName || !pp->envp.envOutputDir || !pp->envp.envFileBaseName || !pp->envp.envStdOutFileName)
	{
		printf("  Well ... it happens. Failed to allocate memory for file names\n");
		clearProgramParameters(pp);
		return NULL;
	}
	
	/* Since here all stdout data will be writen to file */
		
	if(pp->envp.envStdOutMode)
		if(createNewStdOut(pp) == -1)
			printf(" !! StdOut mode not supported !!\n");

	
	pp->envp.envForce 			= lookForInt("Force");	
	pp->nnp.nnLearnRate 		= lookForDouble("LearnRate");
	pp->nnp.nnMomentumRate	=	lookForDouble("MomentumRate");
	pp->nnp.nnConnectionsInsideInput = lookForInt("ConnectionsInsideInput");
	pp->nnp.nnConnectionsInsideOutput = lookForInt("ConnectionsInsideOutput");
	pp->nnp.nnSignalPropagation = lookForInt("SignalPropagation");
	pp->nnp.nnACFMode 			= lookForInt("ActivationFunctionMode");
	
	if(pp->nnp.nnACFMode == 0)
		printf("  Warning! Raw activation function will be used!\n");
	
	iVal = regNodeSize("ActivationFunctionParametersList");
	if(iVal <= 0)
	{
		printf("  Warning! List of Activation Function parameters is empty, using defaults!\n");
		iVal = regNodeSize("defaultActivationFunctionParametersList");
		strcpy(tmpLine,"defaultActivationFunctionParametersList");
	}
	else
		strcpy(tmpLine,"ActivationFunctionParametersList");

	//	be carefull, parameters list has alway one parameter, the number of parameters
	pp -> nnp.nnAFPCnt = iVal;
	pp -> nnp.nnAFP = (double*)_malloc(sizeof(double)*(iVal+1));
	assert(pp -> nnp.nnAFP != NULL);
	if(pp->nnp.nnAFP)
	{
		pp->nnp.nnAFP[0] = iVal + 1;
		for(int i=0;i<iVal;i++)
			pp->nnp.nnAFP[i+1] = regListDouble(tmpLine,i);
		pp->nnp.nnAFPCnt = iVal;
	}
	else
	{
		printf("	!! Failed to allocate memory, quiting !!\n");
		clearProgramParameters(pp);
		return NULL;
	}
	
	iVal = regNodeSize("ActivationDerivativeParametersList");
	if(iVal <= 0)
	{
		printf("  Warning! List of Activation Derivative parameters is empty, using defaults!\n");
		iVal = regNodeSize("defaultActivationDerivativeParametersList");
		strcpy(tmpLine,"defaultActivationDerivativeParametersList");
	}
	else
		strcpy(tmpLine,"ActivationDerivativeParametersList");

	pp->nnp.nnADP = (double*)_malloc(sizeof(double)*(iVal+1));
	assert(pp->nnp.nnADP != NULL);
	if(pp->nnp.nnADP)
	{
		pp->nnp.nnADP[0] = iVal + 1;
		for(int i=0;i<iVal;i++)
			pp->nnp.nnADP[i+1] = regListDouble(tmpLine,i);
		pp->nnp.nnADPCnt = iVal;
	}
	else
	{
		printf(" !! Failed to allocate memory, quiting !! \n");
		clearProgramParameters(pp);
		return NULL;
	}
	
	iVal = regNodeSize("FitnessParametersBad");
	if(iVal <= 0)
	{
		printf("  Warning! List of Fitness Function Parameters \"Bad\" is empty!\n");
		iVal = regNodeSize("defaultFitnessParametersBad");
		strcpy(tmpLine,"defaultFitnessParametersBad");
	}
	else
		strcpy(tmpLine,"FitnessParametersBad");

	pp->nnp.nnFitnessParametersBad = (double*)_malloc(sizeof(double)*iVal);
	assert(pp->nnp.nnFitnessParametersBad != NULL);
	if(pp->nnp.nnFitnessParametersBad)
	{
		pp->nnp.nnFitnessParametersBadCnt = iVal;
		for(int i=0;i<iVal;i++)
			pp->nnp.nnFitnessParametersBad[i] = regListDouble(tmpLine,i);
	}
	else
	{
		printf(" !! Failed to allocate memory, quiting !!\n");
		clearProgramParameters(pp);
		return NULL;
	}
	
	iVal = regNodeSize("FitnessParametersGood");
	if(iVal <= 0)
	{
		printf("  Warning! List of Fitness Function Parameters \"Good\" is empty!\n");
		iVal = regNodeSize("defaultFitnessParametersGood");
		strcpy(tmpLine,"defaultFitnessParametersGood");
	}
	else
		strcpy(tmpLine,"FitnessParametersGood");

	pp->nnp.nnFitnessParametersGood = (double*)_malloc(sizeof(double)*iVal);
	assert(pp->nnp.nnFitnessParametersGood != NULL);
	if(pp->nnp.nnFitnessParametersGood)
	{
		pp->nnp.nnFitnessParametersGoodCnt = iVal;
		for(int i=0;i<iVal;i++)
			pp->nnp.nnFitnessParametersGood[i] = regListDouble(tmpLine,i);
	}
	else
	{
		printf(" !! Failed to allocate memory, quiting !!\n");
		clearProgramParameters(pp);
		return NULL;
	}
	
	iVal = regNodeSize("FitnessParametersLearned");
	if(iVal <= 0)
	{
		printf("  Warning! List of Fitness Function Parameters \"Learned\" is empty!\n");
		iVal = regNodeSize("defaultFitnessParametersLearned");
		strcpy(tmpLine,"defaultFitnessParametersLearned");
	}
	else
		strcpy(tmpLine,"FitnessParametersLearned");

	pp->nnp.nnFitnessParametersLearned = (double*)_malloc(sizeof(double)*iVal);
	assert(pp->nnp.nnFitnessParametersLearned != NULL);
	if(pp->nnp.nnFitnessParametersLearned)
	{
		pp->nnp.nnFitnessParametersLearnedCnt = iVal;
		for(int i=0;i<iVal;i++)
			pp->nnp.nnFitnessParametersLearned[i] = regListDouble(tmpLine,i);
	}
	else
	{
		printf(" !! Failed to allocate memory, quiting !!\n");
		clearProgramParameters(pp);
		return NULL;
	}

	pp->nnp.nnDataMaxPresentationsCnt = lookForInt("DataMaximalPresentationsCount");
	pp->nnp.nnMaximalError 						= lookForDouble("MaximalError");
	pp->nnp.nnRandomPattern						= lookForInt("RandomPattern");
 	pp->nnp.nnWeightModificationMethod= lookForInt("WeightModificationMethod");
  pp->nnp.nnQuickPropParameter			= lookForDouble("QuickPropParameter");
 	 	
	dVal = lookForDouble("LeftWeight");
 	dVal2 = lookForDouble("RightWeight");
 	if(dVal >= dVal2 || (dVal == 0 && dVal2 == 0))
 	{
		printf("Setting weights to defaults\n");
 		dVal 	= regDouble("defaultLeftWeight");
 		dVal2 = regDouble("defaultRightWeight");
 	}
 	pp->nnp.nnLeftWeight 	= dVal;
 	pp->nnp.nnRightWeight = dVal2;
	pp->nnp.nnBiasMode 		= lookForDouble("BiasMode");	
	if(pp->nnp.nnBiasMode == 0)		
		printf("  Warning! Bias will be turned off!\n");
	pp->nnp.nnActivationThreshold = lookForInt("ActivationThreshold");

	iVal = lookForInt("LearnDataCnt");
	if(iVal <= 0)
	{
		printf("	You have specified wrong number of Learn Data. I'll determine it from the file\n");
		pp->nnp.nnLearnDataCnt = 0;
	}
	else
		pp->nnp.nnLearnDataCnt = iVal;

	iVal = lookForInt("TestDataCnt");
	if(iVal <= 0)
	{
		printf("	You have specified wrong number of Test Data. I'll determine it from the file\n");
		pp->nnp.nnTestDataCnt = 0;
	}
	else
		pp->nnp.nnTestDataCnt = iVal;		

	iVal = lookForInt("InitMode");
	if(iVal < 0)
	{
		printf("  You have specified unsupported mode. Setting to defaults\n");
		pp->gap.gaInitMode = 0;
	}
	else
	{
		printf("  Init mode not supported yet\n");
  	pp->gap.gaInitMode 	= iVal;
	}
	
	
	pp->gap.gaCrossOver 	= lookForDouble("CrossOverParameter");
	pp->gap.gaMutation 		= lookForDouble("MutationParameter");
	
	pp->gap.gaNormalization 	= lookForInt("Normalization");
	pp->gap.gaLeftNorm 				= lookForDouble("LeftNorm");
	pp->gap.gaRightNorm				= lookForDouble("RightNorm");
	
	pp->gap.gaFitnessByPosition=lookForInt("FitnessByPosition");

  iVal = lookForInt("EpochsCount");
	if(iVal > 0)
	 	pp->gap.gaEpochsCnt = iVal;
	else
	{
		printf("  Epochs count shuld be greater then 0, setting to defaults\n");
		pp->gap.gaEpochsCnt = regInt("defaultEpochsCount");
	}
	iVal = lookForInt("IndividualsCount");
  if(iVal > 0)
  {
		while(iVal % processCnt)  	
			iVal ++;
		pp->gap.gaIndividualsCnt = iVal;
	}
	else
	{
		printf(" Individuals count shuld be greater then 0, setting to defaults\n");
		pp->gap.gaIndividualsCnt = regInt("defaultIndividualsCount");
		while(pp->gap.gaIndividualsCnt % processCnt)
			pp->gap.gaIndividualsCnt ++;
	}

	pp->gap.gaPopulationLogCnt = lookForInt("PopulationLogCount");
	if(pp->gap.gaPopulationLogCnt < 0 || pp->gap.gaPopulationLogCnt > pp->gap.gaIndividualsCnt / processCnt)
		pp->gap.gaPopulationLogCnt = pp->gap.gaIndividualsCnt / processCnt;

	iVal = lookForInt("GenotypeSize");
	if(iVal > 0)
		pp->gap.gaGenotypeSize = iVal;
	else
	{
		printf("  Genotype size shuld be greater then 0, setting to defaults\n");
		pp->gap.gaGenotypeSize = lookForInt("GenotypeSize");
	}

	pp->envp.envTestNetwork 				= lookForInt("TestNetwork");
	pp->envp.envShowLearningProcess = lookForInt("ShowLearningProcess");
	
	dVal = lookForDouble("TransferParameter");
  if(dVal > 0)
		pp->gap.gaTransferParameter = dVal;
	else
	{
		printf("  Transfer parameter shuld be greater then 0, setting to defaults\n");
		pp->gap.gaTransferParameter = regDouble("defaultTransferParameter");;
	}
	
	iVal = lookForInt("CommunicationEpoch");
	if(iVal > 0)
		pp->gap.gaCommunicationEpoch = iVal;
	else	
	{
		printf("  Communication epoch shuld be greater then 0, setting to defaults\n");
		pp->gap.gaCommunicationEpoch = regInt("defaultCommunicationEpoch");
	}	
	iVal = lookForInt("FitnesCount");
	if(iVal > 0)
		pp->gap.gaFitCnt = iVal;
	else
	{
		printf("  You shuld type in value greater then 0, using defaults\n");
		pp->gap.gaCommunicationEpoch = regInt("defaultFitnesCount");
	}
	
	pp->parp.parMeshWidth					= lookForInt("MeshWidth");
	pp->parp.parMeshHeight				= lookForInt("MeshHeight");
	if(pp->parp.parMeshWidth * pp->parp.parMeshHeight != processCnt || pp->parp.parMeshWidth <= 0 || pp->parp.parMeshHeight <= 0)
	{
		int found = 0;
		int maxw = 1;			// thous are the best values for width and height of the mesh
		int maxh = 1;
		printf("  The size of mesh is illegal. Trying to improve the values !\n");
		for(int i=1;i<=processCnt;i++)
		{
			for(int p=1;p<=processCnt;p++)
			{
				if(i*p == processCnt)
				{
					printf("  w=%d h=%d seems to be good\n",i,p);
					if(!found)
					{
						maxw = i;
						maxh = p;						
						found = 1;
					}				
					else
					if(abs(i-p) < abs(maxw-maxh))
					{
						maxw = i;
						maxh = p;
					}
				}
			}		
		}
		if(!found)
		{
			printf("  It's not good. I'am unable to set width and height of mesh\n");
			return NULL;
		}		
		printf("  Probably the best pair is: w=%02d h=%02d\n",maxw,maxh);
		pp->parp.parMeshWidth				= maxw;
		pp->parp.parMeshHeight			= maxh;
	}
	
	pp->envp.envLogSize						= lookForInt("LogSize");	
	
	tmpFile = open(pp->envp.envLearnDataFileName,O_RDONLY);
	if(tmpFile == -1)
	{
		printf("Oh, no. Learn data file wanished.\n");
		printf("  I just can't emagine you badly typed in the file name\n");
		clearProgramParameters(pp);
		return NULL;
	}
		
	int lineNo = 0;
	while(tmpFile && readLine(tmpFile,line))
	{
		if(line[0])
		{
			lineNo ++;
			strcpy(tmpLine,line);
			tokens = getTokens(line);
	 		int tokensCnt = getTokensSize(tokens);
  		clearTokens(tokens);		
	  	if(tokensCnt != (inputs + outputs))
		  {
				printf("From learn data file\n");
				printf("%s ",tmpLine);
				printf("\n  Well. For me it's not exactly what you ment to do.\n");
				printf("  I'll try to be helpful. Line number is: %d\n",lineNo);				
  			clearProgramParameters(pp);
	  		return NULL;
		  }
  		else
	  	{
		  	if(!addLearnData(pp->ds,tmpLine))
  			{
					printf("Failed to add Learn data. Yes, yes, this time it's my fault\n");
	  			clearProgramParameters(pp);
		  		return NULL;
  			}
	  	}
  	}
  }
	if(lineNo)
		printf("%d - Learn Data patterns read\n",lineNo);
	else
		printf("Learn Data File seems to be empty ... it's not good idea\n");
	close(tmpFile);
	
	tmpFile = open(pp->envp.envTestDataFileName,O_RDONLY);
	if(tmpFile == -1)
	{
		printf("Oh, no. Test data file wanished.\n");
		printf("  I just can't emagine you badly typed in the file name\n");
		clearProgramParameters(pp);
		return NULL;
	}

	lineNo = 0;	
	while(readLine(tmpFile,line))
	{
	  if(line[0])
	  {
			lineNo ++;
  		strcpy(tmpLine,line);
	  	tokens = getTokens(line);
		  int tokensCnt = getTokensSize(tokens);
  		clearTokens(tokens);
	  	if(tokensCnt != (inputs + outputs))
		  {
	      printf("From test data file\n");
				printf("%s ",tmpLine);
				printf("\n  Well. For me it's not exactly what you ment to do.\n");
				printf("  I'll try to be helpful. Line number is: %d\n",lineNo);				
			  clearProgramParameters(pp);
  			return NULL;
	  	}
		  else
  		{
	  		if(!addTestData(pp->ds,tmpLine))
		  	{
					printf("Memory allocation error. Test data can not be added.\n");
					printf("  Trying to be helpfull. Buy more base memory\n");
			  	clearProgramParameters(pp);
				  return NULL;
  			}
	  	}
  	}
  }

 	if(lineNo)
		printf("%d - Test Data patters read\n",lineNo);
	else
		printf("Test Data file seems to be empty. Are you sure it's all right\n");
	close(tmpFile);
	

	
	return pp;
}

ProgramParameters *getProgramParameters()
{
	return programParameters;
}
