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
	activationFunc = new CActivationFunctionSigmoid();
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
	m_Output = activationFunc->Activate( output );
}

void CNeuronSigmoid::getWeights( vector<double> & weights )
{
	weights = m_Weights;
}

void CNeuronSigmoid::setWeights( vector<double> & weights )
{
	m_Weights = weights;
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
