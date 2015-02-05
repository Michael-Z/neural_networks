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

double CActivationFunctionSigmoid::Activate( double value )
{
	double result = 1 / ( 1 + exp( ( -1 )*value ) );

	return result;
}
