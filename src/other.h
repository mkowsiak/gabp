#ifndef _OTHER_H
#define _OTHER_H

#include "monitor.h"

double sqr(double);
//double abs(double);
char *ffBuffer(char *);
int numberSequenceSum(int );
char **getTokens(char *);
int getTokensSize(char **);
void clearTokens(char**);
char *_strdup(char *);
char *findEnvironmentString(char **,char *);
void shuffle(int *,int );
int createNewStdOut(ProgramParameters*);
void GABP_abort(int);

#endif
