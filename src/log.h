#ifndef _LOG_H
#define _LOG_H

#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>

#define LOG_ERROR 			0x0000
#define LOG_ERROR_COMMUNICATION_ERROR 		0x0001
#define LOG_ERROR_CONNECTION_ERROR 				0x0002
#define LOG_ERROR_MEMORY_ALLOCATION_ERROR 0x0003
#define LOG_ERROR_COMPUTE_ERROR						0x0004
#define LOG_COMM_START 	0xff01
#define LOG_COMM_END   	0xff02
#define LOG_COMP_START 	0xff03
#define LOG_COMP_END		0xff04
#define LOG_IDLE_START	0xff05
#define LOG_IDLE_END		0xff06
#define LOG_GA_INDIVIDUAL_FIT	0x0010
#define LOG_GA_INDIVIDUAL_GENOTYPE_O 0x0011
#define LOG_GA_INDIVIDUAL_GENOTYPE_C 0x0012
#define LOG_GA_POPULATION_AVG_FIT	0x0023
#define LOG_GA_POPULATION_MIN_FIT	0x0024
#define LOG_GA_POPULATION_MAX_FIT	0x0025
#define LOG_FILE_WRITE_START	0xff30
#define LOG_FILE_WRITE_END		0xff31
#define LOG_INFO				0x0040
#define LOG_TIME_STAMP	0xff50
#define LOG_TIME_STAMP_START 0xff51
#define LOG_TIME_STAMP_END		0xff52
#define LOG_GA_ADAPTED_INDIVIDUAL_FIT 0x0060
#define LOG_GA_ADAPTED_INDIVIDUAL_GENOTYPE_O 0x0061
#define LOG_GA_ADAPTED_INDIVIDUAL_GENOTYPE_C 0x0062



struct LogData
{
	int 	logMode;
	char 	*logName;
	double logVal;
	//struct timeval time;	
	double time;
};

struct Log
{
	int logDataCnt;
	int logIndex;
	FILE *logFile;
	LogData *logData;
};

Log *createLog(int,char *);
void clearLog(Log *);
void writeLog(Log *);
Log *LOG(int ,char *,double val = 0);
Log *getProgramLog();


#endif