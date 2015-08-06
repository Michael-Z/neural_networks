/*
 * CActivationFunctionSigmoid.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CActivationFunctionSigmoid.h"
#include "cmath"

CActivationFunctionSigmoid::CActivationFunctionSigmoid()
{
	// TODO Auto-generated constructor stub
}

CActivationFunctionSigmoid::~CActivationFunctionSigmoid()
{
	// TODO Auto-generated destructor stub
}

double CActivationFunctionSigmoid::Function( double value )
{
	double result = 1 / ( 1 + exp( ( -1 )*value ) );

	return result;
}

double CActivationFunctionSigmoid::Derivative( double value )
{
	return value * ( 1 - value );
}
