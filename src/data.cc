#include <assert.h>
#include <stdio.h>
#include "other.h"
#include "data.h"
#include "memory.h"

DataSet *createDataSet(int in,int out)
{
	DataSet *set = (DataSet*) _malloc(sizeof(DataSet));
	assert(set != NULL);
	if(!set)
		return NULL;
	set -> data = NULL;
	set -> test = NULL;
	set -> in = in;
	set -> out = out;
	set -> dataCnt = 0;
	set -> testCnt = 0;
	return set;
}

/*
	We will try to add some learn data. ptrIn points at inputs, and ptrOut at outputs
	Memory reallocation is done, and data are added into set	
*/

DataSet *addLearnData(DataSet *set,double *ptrIn,double *ptrOut)
{
	assert(set != NULL);
	if(set)
	{
		// memory reallocation. we have one more data set
		set -> data = (Data*)_realloc(set->data,sizeof(Data)*(set->dataCnt + 1));
		assert(set->data != NULL);
		if(set->data)
		{
			// memory allocation for inputs
			set -> data[set -> dataCnt].inputs = (double*)_malloc(sizeof(double)*set->in);
			assert(set->data[set->dataCnt].inputs != NULL);
			// memory allocation for outputs
			set -> data[set -> dataCnt].outputs = (double*)_malloc(sizeof(double)*set->out);
			assert(set->data[set->dataCnt].outputs != NULL);
			if(set -> data[set->dataCnt].inputs && set -> data[set->dataCnt].outputs)
			{
				// put data into propper place
				for(int i=0;i<set->in;i++)
					set -> data[set -> dataCnt].inputs[i] = ptrIn[i];
				for(int i=0;i<set->out;i++)
					set -> data[set -> dataCnt].outputs[i] = ptrOut[i];
				set -> dataCnt ++;
			}
			else
			{
				_free((void*)set -> data[set->dataCnt].inputs);
				_free((void*)set -> data[set->dataCnt].outputs);
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}
	return set;
}

/*
	As above, but we'll try to add test data
*/

DataSet *addTestData(DataSet *set,double *ptrIn,double *ptrOut)
{
	assert(set != NULL);
	if(set)
	{
		set -> test = (Data*)_realloc(set->test,sizeof(Data)*(set->testCnt + 1));
		assert(set->test != NULL);
		if(set->test)
		{
			set -> test[set -> testCnt].inputs = (double*)_malloc(sizeof(double)*set->in);
			assert(set -> test[set -> testCnt].inputs != NULL);
			set -> test[set -> testCnt].outputs = (double*)_malloc(sizeof(double)*set->out);
			assert(set -> test[set -> testCnt].outputs != NULL);
			if(set -> test[set->testCnt].inputs && set -> test[set->testCnt].outputs)
			{
				for(int i=0;i<set->in;i++)
					set -> test[set -> testCnt].inputs[i] = ptrIn[i];
				for(int i=0;i<set->out;i++)
					set -> test[set -> testCnt].outputs[i] = ptrOut[i];
				set -> testCnt ++;
			}
			else
			{
				_free((void*)set -> test[set->testCnt].inputs);
				_free((void*)set -> test[set->testCnt].outputs);
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}
	return set;
}

DataSet *addLearnData(DataSet *set,char *str)
{
	double *in;
	double *out;
	int znacznik = 0;
	assert(set != NULL);
	if(set)
	{
		in = (double*) _malloc(sizeof(double)*set->in);
		assert(in != NULL);
		out = (double*)_malloc(sizeof(double)*set->out);
		assert(out != NULL);
		if(in && out)
		{
			for(int i=0;i<set->in && znacznik != 1;i++)
			{
				if(sscanf(str,"%lf ",&in[i]) != 1)   // we are looking for some numbers, a float numbers
					znacznik = 1;
				str = ffBuffer(str);			// this is very nasty function, use with care, and remember ... str will be destroyed
			}
			for(int i=0;i<set->out && znacznik != 1;i++)
			{
				if(sscanf(str,"%lf ",&out[i]) != 1)
					znacznik = 1;
				str = ffBuffer(str);			// as above (take a look at other.cc)
			}
			if(!addLearnData(set,in,out) && znacznik != 1)
				znacznik = 1;
			_free((void*)in);
			_free((void*)out);
			if(znacznik == 1)
				return NULL;
			return set;
		}
		else
		{
			_free((void*)in);
			_free((void*)out);
			return NULL;
		}
	}
	else
		return NULL;
}

/*
	As above, but for test data
*/

DataSet *addTestData(DataSet *set,char *str)
{
	double *in;
	double *out;
	int znacznik = 0;
	assert(set != NULL);
	if(set)
	{
		in = (double*) _malloc(sizeof(double)*set->in);
		assert(in != NULL);
		out = (double*)_malloc(sizeof(double)*set->out);
		assert(out != NULL);
		if(in && out)
		{
			for(int i=0;i<set->in && znacznik != 1;i++)
			{
				if(sscanf(str,"%lf ",&in[i]) != 1)
					znacznik = 1;
				str = ffBuffer(str);
			}
			for(int i=0;i<set->out && znacznik != 1;i++)
			{
				if(sscanf(str,"%lf ",&out[i]) != 1)
					znacznik = 1;
				str = ffBuffer(str);
			}
			if(!addTestData(set,in,out) && znacznik != 1)
				znacznik = 1;
			_free((void*)in);
			_free((void*)out);
			
			if(znacznik == 1)
				return NULL;
			return set;
		}
		else
		{
			_free((void*)in);
			_free((void*)out);
			return NULL;
		}
	}
	else
		return NULL;
}

/*
	From the darknes of data set.
	Here they come. The Learn Pattern Input.
	Using this function you can gat, what you want.
*/

double *getLearnInput(DataSet *set,int no)
{
	assert(set != NULL);
	if(no >= 0 && no < set->dataCnt)
	{
		return set -> data[no].inputs;	// DANGEROUS !! This is a pointer to a true data.
																		// do not free it ...
	}	
	else
		return NULL;
}

/*
	As above but you'll get some output
*/

double *getLearnOutput(DataSet *set,int no)
{
	assert(set != NULL);
	if(no >= 0 && no < set->dataCnt)
	{
		return set -> data[no].outputs;
	}
	else
		return NULL;
}

/*
	Symetric for test data
*/

double *getTestInput(DataSet *set,int no)
{
	assert(set != NULL);
	if(no >= 0 && no < set->testCnt)
	{
		return set -> test[no].inputs;
	}
	else
		return NULL;
}

double *getTestOutput(DataSet *set,int no)
{
	assert(set != NULL);
	if(no >= 0 && no < set->testCnt)
	{
		return set -> test[no].outputs;
	}
	else
		return NULL;
}

/*
	setting the parameter inside of the structure
*/

void setLearnTime(DataSet *data,int time)
{
	data->learnTime = time;	
}

/*
	as above
*/

void setMaxError(DataSet *data,double error)
{
	data->maxError = error;
}

/*
	We definitely like to sweap.
*/

void clearDataSet(DataSet *data)
{
	 if(data)
	 {
	 		for(int i=0;i<data->dataCnt;i++)
	 		{
	 			_free(data->data[i].inputs);
	 			_free(data->data[i].outputs);
	 		}
	 		for(int i=0;i<data->testCnt;i++)
	 		{
	 			_free(data->test[i].inputs);
	 			_free(data->test[i].outputs);
	 		}
	 		data->dataCnt = 0;
	 		data->testCnt = 0;
	 		_free(data->test);
	 		_free(data->data);
	 		_free(data);
	 		data=NULL;
	 }
}
