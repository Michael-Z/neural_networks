/*
 * CNeuronData.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CNeuronData.h"
#include "CActivationFunctionSigmoid.h"

CNeuronData::CNeuronData(): m_Output(0.0 )
{
}

CNeuronData::~CNeuronData()
{
}

double CNeuronData::getOutput()
{
	return m_Output;
}

void CNeuronData::setOutput( double output )
{
	m_Output = output;
}

double CNeuronData::calculateDerivative( double value )
{
	return 0;
}

vector<Weight> & CNeuronData::getWeights()
{
	return m_Weights;
}

double CNeuronData::getDelta()
{
	return 0.0;
}

void CNeuronData::setDelta( double delta )
{
}

double CNeuronData::getError()
{
	return 0.0;
}

void CNeuronData::setError( double error )
{
}

double CNeuronData::getGradient()
{
	return 0;
}

void CNeuronData::setGradient( double gradient )
{
	;
}
