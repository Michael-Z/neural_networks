/*
 * CNeuronData.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CNeuronData.h"
#include "CActivationFunctionSigmoid.h"

CNeuronData::CNeuronData(): m_Output(0.0 ), m_Delta( 0.0 ), m_Error( 0.0 )
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

void CNeuronData::getWeights( vector<double> & weights )
{
	weights = m_Weights;
}

void CNeuronData::setWeights( vector<double> & weights )
{
	m_Weights = weights;
}

double CNeuronData::getDelta()
{
	return m_Delta;
}

void CNeuronData::setDelta( double delta )
{
	m_Delta = delta;
}

double CNeuronData::getError()
{
	return m_Error;
}

void CNeuronData::setError( double error )
{
	m_Error = error;
}
