#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "memory.h"
#include "activation_functions.h"

/*

	Name:						createActivationFunction
	Description:		creats activation function depending on value isKindOf
	Arguments:			isKindOf - the kind of function
									fP,dP - pointers to function arguments
	Returned Value:	ActivationFunction structure
	
*/

ActivationFunction *createActivationFunction(int isKindOf,double *fP,double *dP)
{
	switch(isKindOf)
	{
		case FUNCTION_RAW:									// just the net value
			return createRaw();
		case FUNCTION_UNIPOLAR:							
			return createUnipolar();
		case FUNCTION_BIPOLAR:
			return createBipolar();
		case FUNCTION_UNISIGMOIDAL:
			return createUnisigmoidal(fP,dP);
		case FUNCTION_BISIGMOIDAL:
			return createBisigmoidal(fP,dP);
		case FUNCTION_LOGARITHMIC:
			return createLogarithmic(fP,dP);
		default:
			return createRaw();
	}
}

double rawFunction(double *p,double dVal)
{
	return dVal;
}

double rawDerivative(double *p,double dVal)
{
	return 0;
}

ActivationFunction *createRaw(double *fP,double *dP)
{
	ActivationFunction *newF = (ActivationFunction*)_malloc(sizeof(ActivationFunction));
	assert(newF != NULL);
	if(newF)
	{
		newF->functionParameters = NULL;
		newF->function = rawFunction;
		newF->derivativeParameters=NULL;
		newF->derivative = rawDerivative;
	}
	return newF;
}

double bipolarFunction(double *p,double dVal)
{
	if(dVal >= 0)
	{
		return 1;		
	}
	else
	{
		return -1;
	}
}

double bipolarDerivative(double *p,double dVal)
{
	return 0;
}

ActivationFunction *createBipolar(double *fP,double *dP)
{
	ActivationFunction *newF = (ActivationFunction*)_malloc(sizeof(ActivationFunction));
	assert(newF != NULL);
	if(newF)
	{
		newF->functionParameters = NULL;
		newF->function = rawFunction;
		newF->derivativeParameters=NULL;
		newF->derivative = rawDerivative;
	}
	return newF;
}

double unipolarFunction(double *p,double dVal)
{
	if(dVal >= 0)
	{
		return 1;		
	}
	else
	{
		return 0;
	}
}

double unipolarDerivative(double *p,double dVal)
{
	return 0;
}

ActivationFunction *createUnipolar(double *fP,double *dP)
{
	ActivationFunction *newF = (ActivationFunction*)_malloc(sizeof(ActivationFunction));
	assert(newF != NULL);
	if(newF)
	{
		newF->functionParameters = NULL;
		newF->function = rawFunction;
		newF->derivativeParameters=NULL;
		newF->derivative = rawDerivative;
	}
	return newF;
}

double unisigmoidalFunction(double *p,double dVal)
{
	return (double)1/( (double)1+exp(-(p[2]*(dVal+p[1]))));
}

double unisigmoidalDerivative(double *p,double dVal)
{
	return (double)p[2]*unisigmoidalFunction(p,dVal)*((double)1-unisigmoidalFunction(p,dVal));
}

ActivationFunction *createUnisigmoidal(double *fP,double *dP)
{
	ActivationFunction *newF = (ActivationFunction*)_malloc(sizeof(ActivationFunction));
	assert(newF != NULL);
	if(newF)
	{
		if(fP)		// if parameters were given try to allocate memory for them
		{
			// remember, one more for parameters count. it will be used, maybe
			newF->functionParameters = (double*)_malloc(sizeof(double)*(unsigned int)fP[0]);
			assert(newF->functionParameters != NULL);
			if(newF->functionParameters)
			{
				for(int i=0;i<fP[0];i++)
					newF->functionParameters[i] = fP[i];  	// parameters for function
			}
		}
		if(dP)
		{
			newF->derivativeParameters = (double*)_malloc(sizeof(double)*(unsigned int)dP[0]);
			assert(newF->derivativeParameters != NULL);
			if(newF->derivativeParameters)
			{
				for(int i=0;i<dP[0];i++)
					newF->derivativeParameters[i] = dP[i];		// parameters for derivative
			}
		}
		if(newF->functionParameters && newF->derivativeParameters)
		{
			newF->function = unisigmoidalFunction;			// everything is ok
			newF->derivative = unisigmoidalDerivative;  // associate pointers with proper functions
		}
		else
		{
			_free((void*)newF->functionParameters);			// some problems
			_free((void*)newF->derivativeParameters);
			_free((void*)newF);
			return NULL;
		}
	}
	return newF;
}

double bisigmoidalFunction(double *p,double dVal)
{
	return 2*unisigmoidalFunction(p,dVal) - 1;
}

double bisigmoidalDerivative(double *p,double dVal)
{
	return (double)2*p[2]*unisigmoidalFunction(p,dVal)*((double)1-unisigmoidalFunction(p,dVal));
}

ActivationFunction *createBisigmoidal(double *fP,double *dP)
{
	ActivationFunction *newF = (ActivationFunction*)_malloc(sizeof(ActivationFunction));
	assert(newF != NULL);
	if(newF)
	{
		if(fP)
		{
			newF->functionParameters = (double*)_malloc(sizeof(double)*(unsigned int)fP[0]);
			assert(newF->functionParameters != NULL);
			if(newF->functionParameters)
			{
				for(int i=0;i<fP[0];i++)
					newF->functionParameters[i] = fP[i];
			}
		}
		if(dP)
		{
			newF->derivativeParameters = (double*)_malloc(sizeof(double)*(unsigned int)dP[0]);
			
			assert(newF->derivativeParameters != NULL);
			
			if(newF->derivativeParameters)
			{
				for(int i=0;i<dP[0];i++)
					newF->derivativeParameters[i] = dP[i];
			}
		}
		if(newF->functionParameters && newF->derivativeParameters)
		{
			newF->function = bisigmoidalFunction;
			newF->derivative = bisigmoidalDerivative;
		}
		else
		{
			_free((void*)newF->functionParameters);
			_free((void*)newF->derivativeParameters);
			_free((void*)newF);
			return NULL;
		}
	}
	return newF;
}



double logarithmicFunction(double *p,double dVal)
{
	if(dVal >= 0)
		return log(1+p[1]*dVal);
	else
		return -log(1-p[1]*dVal);
}

double logarithmicDerivative(double *p,double dVal)
{
	if(dVal >= 0)
		return p[1]/(1+p[1]*dVal);
	else
		return p[1]/(1-p[1]*dVal);
}

ActivationFunction *createLogarithmic(double *fP,double *dP)
{
	ActivationFunction *newF = (ActivationFunction*)_malloc(sizeof(ActivationFunction));
	assert(newF != NULL);
	if(newF)
	{
		if(fP)
		{
			newF->functionParameters = (double*)_malloc(sizeof(double)*(unsigned int)fP[0]);
			assert(newF->functionParameters != NULL);
			if(newF->functionParameters)
			{
				for(int i=0;i<fP[0];i++)
					newF->functionParameters[i] = fP[i];
			}
		}
		if(dP)
		{
			newF->derivativeParameters = (double*)_malloc(sizeof(double)*(unsigned int)dP[0]);
			
			assert(newF->derivativeParameters != NULL);
			
			if(newF->derivativeParameters)
			{
				for(int i=0;i<dP[0];i++)
					newF->derivativeParameters[i] = dP[i];
			}
		}
		if(newF->functionParameters && newF->derivativeParameters)
		{
			newF->function = logarithmicFunction;
			newF->derivative = logarithmicDerivative;
		}
		else
		{
			_free((void*)newF->functionParameters);
			_free((void*)newF->derivativeParameters);
			_free((void*)newF);
			return NULL;
		}
	}
	return newF;
}
