#ifndef _ACTIVATION_FUNCTIONS_H
#define	_ACTIVATION_FUNCTIONS_H

#include <stdio.h>

#define FUNCTION_RAW   		0
#define FUNCTION_UNIPOLAR 1
#define FUNCTION_BIPOLAR	2
#define FUNCTION_UNISIGMOIDAL 3
#define FUNCTION_BISIGMOIDAL	4
#define FUNCTION_LOGARITHMIC	5


/*
	parameters description:
	
	offset		description
	
	0					count of parameters
	1					parameter no 1
	2					parameter no 2
	..
	n					parameter no n
*/

struct ActivationFunction
{
	double *functionParameters;
	double *derivativeParameters;
	double (*function)(double *,double);
	double (*derivative)(double *,double);
};

ActivationFunction *createActivationFunction(int,double*,double* );

double rawFunction(double *,double);
double rawDerivative(double *,double);
ActivationFunction *createRaw(double *fP=NULL,double *dP=NULL);

double bipolarFunction(double *,double);
double bipolarDerivative(double *,double);
ActivationFunction *createBipolar(double *fP=NULL,double *dP=NULL);

double unipolarFunction(double *,double);
double unipolarDerivative(double *,double);
ActivationFunction *createUnipolar(double *fP=NULL,double *dP=NULL);

double unisigmoidalFunction(double *,double );
double unisigmoidalDerivative(double *,double );
ActivationFunction *createUnisigmoidal(double *fP=NULL,double *dP=NULL);

double bisigmoidalFunction(double *,double);
double bisigmoidalDerivative(double *,double);
ActivationFunction *createBisigmoidal(double *fP=NULL,double *dP=NULL);

double logarithmicFunction(double *,double);
double logarithmicDerivative(double *,double);
ActivationFunction *createLogarithmic(double *fP=NULL,double *dP=NULL);

#endif