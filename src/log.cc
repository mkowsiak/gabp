#include "log.h"
#include "memory.h"
#include "mpi.h"
#include <assert.h>
#include <string.h>

Log *programLog = NULL;

Log *createLog(int logCnt,char *fileName)
{
	int i;
	Log *_new = (Log*)_malloc(sizeof(Log));
	assert(_new!=NULL);
	if(!_new)
		return NULL;
	_new -> logIndex = 0;
	_new -> logDataCnt = logCnt;
	_new -> logData = (LogData*) _malloc(sizeof(LogData)*logCnt);
	assert(_new -> logData);
	if(!_new->logData)
	{
		_free(_new);
		return NULL;
	}
	memset(_new->logData,0,sizeof(LogData)*logCnt);
	for(i=0;i<logCnt;i++)
	{
		_new->logData[i].logName = (char*)_malloc(sizeof(char)*1024);
		assert(_new->logData[i].logName != NULL);
		if(_new->logData[i].logName == NULL)
			break;
  }
	if(i < logCnt)
	{
		clearLog(_new);
		return NULL;
	}
	_new -> logFile = fopen(fileName,"w");
	if(!_new->logFile)
	{
		clearLog(_new);
		return NULL;
	}
	return _new;
}

void clearLog(Log *log)
{
	if(!log)
		return;
	writeLog(log);
	if(log->logFile)
		fclose(log->logFile);
	for(int i=0;i<log->logDataCnt;i++)
		_free(log->logData[i].logName);
	_free(log->logData);
	_free(log);
	log=NULL;
}

Log *LOG(int mode,char *name,double val)
{
	if(!programLog)
			return NULL;
	int index = programLog -> logIndex;
	if(mode & 0xff00)
		programLog -> logData[index].time = MPI_Wtime();		
		//gettimeofday(&(programLog -> logData[index].time),NULL);
	else
		memset(&(programLog -> logData[index].time),0,sizeof(timeval));
	programLog -> logData[index].logMode = mode;
	programLog -> logData[index].logVal = val;
	strcpy(programLog -> logData[index].logName,name);
	programLog->logIndex = index + 1;
	if(programLog->logIndex == programLog->logDataCnt)
		writeLog(programLog);
	return programLog;
}

void writeLog(Log *log)
{
	/*struct timeval start;
	struct timeval end;*/
	double start;
	double end;
	if(!log)
		return;
	start = MPI_Wtime();
	//gettimeofday(&start,NULL);

	for(int i=0;i<log->logIndex;i++)
	{
		switch(log->logData[i].logMode)
		{
			case LOG_ERROR:
				fprintf(log->logFile,"error: ");
				break;
			case LOG_ERROR_COMMUNICATION_ERROR:
				fprintf(log->logFile,"errcm: ");
				break;
			case LOG_ERROR_CONNECTION_ERROR:
				fprintf(log->logFile,"errcn: ");
				break;
			case LOG_ERROR_MEMORY_ALLOCATION_ERROR:
				fprintf(log->logFile,"erral: ");
				break;
			case LOG_ERROR_COMPUTE_ERROR:
				fprintf(log->logFile,"errcp: ");
				break;
			case LOG_COMM_START:
				fprintf(log->logFile,"comm[: ");
				break;
			case LOG_COMM_END:
				fprintf(log->logFile,"comm]: ");
				break;
			case LOG_COMP_START:
				fprintf(log->logFile,"comp[: ");
				break;
			case LOG_COMP_END:
				fprintf(log->logFile,"comp]: ");
				break;
			case LOG_IDLE_START:
				fprintf(log->logFile,"idle[: ");
				break;
			case LOG_IDLE_END:
				fprintf(log->logFile,"idle]: ");
				break;
			case LOG_GA_INDIVIDUAL_FIT:
				fprintf(log->logFile,"fitne: ");
				break;				
			case LOG_GA_INDIVIDUAL_GENOTYPE_O:
				fprintf(log->logFile,"gen_o: ");
				break;
			case LOG_GA_INDIVIDUAL_GENOTYPE_C:
				fprintf(log->logFile,"gen_c: ");
				break;
			case LOG_GA_POPULATION_AVG_FIT:
				fprintf(log->logFile,"avfit: ");
				break;
			case LOG_GA_POPULATION_MIN_FIT:
				fprintf(log->logFile,"mifit: ");
				break;
			case LOG_GA_POPULATION_MAX_FIT:
				fprintf(log->logFile,"mafit: ");
				break;
			case LOG_FILE_WRITE_START:
				fprintf(log->logFile,"wr2f[: ");
				break;
			case LOG_FILE_WRITE_END:
				fprintf(log->logFile,"wr2f]: ");
				break;
			case LOG_INFO:
				fprintf(log->logFile,"info!: ");
				break;
			case LOG_TIME_STAMP:
				fprintf(log->logFile,"tstmp: ");
				break;
			case LOG_TIME_STAMP_START:
				fprintf(log->logFile,"tstm[: ");
				break;
			case LOG_TIME_STAMP_END:
				fprintf(log->logFile,"tstm]: ");
				break;
			case LOG_GA_ADAPTED_INDIVIDUAL_FIT:
				fprintf(log->logFile,"fit_a: ");
				break;
			case LOG_GA_ADAPTED_INDIVIDUAL_GENOTYPE_O:
				fprintf(log->logFile,"genoa: ");
				break;
			case LOG_GA_ADAPTED_INDIVIDUAL_GENOTYPE_C:
				fprintf(log->logFile,"genoc: ");
				break;
			default:
				fprintf(log->logFile,"undef: ");
				break;
		}		
		fprintf(log->logFile,"%f\tdesc: %s\tval: %f\tmode: %x\n",
					log->logData[i].time,
					log->logData[i].logName,log->logData[i].logVal,
					log->logData[i].logMode);
	}	
	
	end = MPI_Wtime();
	
	//gettimeofday(&end,NULL);
	
	log -> logData[0].logMode = LOG_FILE_WRITE_START;
	log -> logData[0].logVal = 0;
	log -> logData[0].time = start;
	strcpy(log -> logData[0].logName,"file write start");
	
	log -> logData[1].logMode = LOG_FILE_WRITE_END;
	log -> logData[1].logVal = 0;
	log -> logData[1].time = end;
	strcpy(log -> logData[1].logName,"file write end");
	
	log -> logIndex = 2;
}

Log *getProgramLog()
{
	return programLog;
}

