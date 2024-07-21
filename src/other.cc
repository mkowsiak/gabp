#include <assert.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "other.h"
#include "memory.h"
#include "monitor.h"
#include "parallel.h"
#include "log.h"

void shuffle(int *tab,int size)
{
	int newPos;
	int tmp;
	for(int i=0;i<size;i++)
	{
		newPos = rand() % size;
		tmp = tab[newPos];
		tab[newPos] = tab[i];
		tab[i] = tmp;				
	}
}

void GABP_abort(int sigNo)
{
	if(sigNo == SIGKILL)
		LOG(LOG_ERROR,"Process has been killed");
	if(sigNo == SIGINT)
		LOG(LOG_ERROR,"Process has been interrupted");
	if(sigNo == SIGABRT)
		LOG(LOG_ERROR,"Process has been aborted");
	/*if(sigNo == SIGHUP)
		LOG(LOG_ERROR,"Process has been hanguped");*/
	if(sigNo == SIGQUIT)
		LOG(LOG_ERROR,"Process has been quited");
	/*if(sigNo == SIGTERM)
		LOG(LOG_ERROR,"Process has been terminated");*/
	/*if(sigNo == SIGTTOU)
		LOG(LOG_ERROR,"Process has been ttyed out");*/
		
	fprintf(stderr,"Aborting !\n");
	GABP_Abort(9);
}

/*double abs(double dVal)
{
	return dVal > 0 ? dVal : - dVal;
}*/

char *ffBuffer(char *ptr)
{
	char *ffPtr = ptr;
	while(*ffPtr != ' ' && *ffPtr != '\n' && *ffPtr != '\t' && *ffPtr != '\0')
		ffPtr ++;
	while((*ffPtr == ' ' || *ffPtr == '\n' || *ffPtr == '\t') && *ffPtr != '\0')
		ffPtr ++;
	return ffPtr;
}


int numberSequenceSum(int number)
{
	//return (2*1+(number-1))*number/2;
	int sum = 0;
	for(int i=0;i<=number;i++)
		sum += i;
	return sum;
}

double sqr(double dVal)
{
	return dVal*dVal;
}

char **getTokens(char *string)
{
	int extractedCnt = 0;
	
	char *extracted;
	char tokenSep[] = " \t=\n,;";
	
	char **tokens = (char**)_malloc(sizeof(char*));
	assert(tokens != NULL);
	if(tokens)
	{	
		extracted = strtok(string,tokenSep);
		while(extracted)
		{
			tokens[extractedCnt] = strdup(extracted);
			extractedCnt ++;
			tokens = (char**) _realloc(tokens,sizeof(char*)*extractedCnt+1);
			assert(tokens != NULL);
			if(!tokens)
				return NULL;
			extracted = strtok(NULL,tokenSep);
		}
		tokens[extractedCnt] = NULL;	
	}
	return tokens;	
}

void clearTokens(char **tokens)
{
	int i = 0;
	if(tokens)
	{
		while(tokens[i])
		{
			_free(tokens[i++]);
		}
		_free(tokens);
	}
}

int getTokensSize(char **tokens)
{
	int i = 0;
	while(tokens[i])
		i++;
	return i;
}

char *_strdup(char *src)
{
	int len = strlen(src) + 1;
	char *dest = (char*) _malloc(len);
	if(dest)
		memcpy(dest,src,len);
	return dest;	
}

char *findEnvironmentString(char **env,char *string)
{
	int i=0;
	while(env[i] != NULL)
	{
		char *pos;
		pos = strstr(env[i],string);
		if(pos)
		{
			return pos+strlen(string);
		}
		i++;
	}
	return NULL;
}

int createNewStdOut(ProgramParameters *pp)
{
	char buf[256];
	char hostName[256];
	int pid = getpid();
	gethostname(hostName,256);
	
	printf("\ni'm trying to do some good stuff\n");
	sprintf(buf,"%s/%s%02d.%s.%09d",	pp->envp.envOutputDir,
																pp->envp.envStdOutFileName,
																pp->parp.parProcessID,
																hostName,
																pid);
	pp->envp.envStdOut = fopen(buf,"w");
	if(pp->envp.envStdOut)
	{
		// redirection of stdout (a litle bit strange, isn't it ?)
		pp->envp.oldStdOut = stdout;	
#ifdef STDOUT_LINUX
		stdout = pp->envp.envStdOut;
		return 1;
#endif

#if defined(STDOUT_HPPA)
	#if	defined(__STDC__)
		__iob[1] = *pp->envp.envStdOut;
		return 1;
	#else
		_iob[1] = *pp->envp.envStdOut;
		return 1;
	#endif
#endif

#ifdef STDOUT_SUN4SOL2
	#if defined(__STDC__)
		__iob[1] = *pp->envp.envStdOut;
		return 1;
	#else
		_iob[1] = *pp->envp.envStdOut;
		return 1;
	#endif
#endif
	}
	else
	{
		printf(" !! Error creating StdOut file !!\n");
		return 0;
	}
	return -1;
}
