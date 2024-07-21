
#ifndef _DATA_H
#define _DATA_H

struct Data
{
	double *inputs;  			// the place where inputs of the network are stored
	double *outputs;			// the place where desider outputs of the network are stored
};

struct DataSet
{
	int in;										// 	how many inputs there is
	int out;									//	how many outputs there is
	int dataCnt;							//	how many learning patterns there is
	int testCnt;							//	how many testing patterns there is
	Data *data;								//	learning patterns storage
	Data *test;								//	testing patterns storage
	unsigned int learnTime;		//	
	double maxError; 					//	maximal error
};



DataSet *createDataSet(int ,int );
void clearDataSet(DataSet *);
DataSet *addLearnData(DataSet *,char *);
DataSet *addTestData(DataSet *,char *);
DataSet *addTestData(DataSet *,double *,double *);
DataSet *addLearnData(DataSet *,double *,double *);
double *getLearnInput(DataSet *,int);
double *getLearnOutput(DataSet *,int);
double *getTestInput(DataSet *,int);
double *getTestOutput(DataSet *,int);
void setLearnTime(DataSet *,unsigned int);
void setMaxError(DataSet *,double);

#endif
