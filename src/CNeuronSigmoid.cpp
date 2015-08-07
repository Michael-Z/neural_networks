/*
 * CNeuronSigmoid.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CNeuronSigmoid.h"
#include "CActivationFunctionSigmoid.h"

CNeuronSigmoid::CNeuronSigmoid(): m_Output(0.0 ), m_Delta( 0.0 ), m_Error( 0.0 )
{
	activationFunc = new CActivationFunctionSigmoid( 0.01 );
}

CNeuronSigmoid::~CNeuronSigmoid()
{
}

double CNeuronSigmoid::getOutput()
{
	return m_Output;
}

void CNeuronSigmoid::setOutput( double output )
{
	m_Output = activationFunc->Function( output );
}

double CNeuronSigmoid::calculateDerivative( double value )
{
	return activationFunc->Derivative( value );
}

vector<double> & CNeuronSigmoid::getWeights()
{
	return m_Weights;
}

double CNeuronSigmoid::getDelta()
{
	return m_Delta;
}

void CNeuronSigmoid::setDelta( double delta )
{
	m_Delta = delta;
}

double CNeuronSigmoid::getError()
{
	return m_Error;
}

void CNeuronSigmoid::setError( double error )
{
	m_Error = error;
}
