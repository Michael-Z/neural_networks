/*
 * CActivationFunctionSigmoid.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CActivationFunctionSigmoid.h"
#include "cmath"

CActivationFunctionSigmoid::CActivationFunctionSigmoid( double alpha ) : m_Alpha( alpha )
{
}

CActivationFunctionSigmoid::~CActivationFunctionSigmoid()
{
}

double CActivationFunctionSigmoid::Function( double value )
{
	double result = 1 / ( 1 + exp( ( -1 )*value*m_Alpha ) );

	return result;
}

double CActivationFunctionSigmoid::Derivative( double value )
{
	return value * ( 1 - value );
}
