/*
 * CNeuronSigmoid.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CNeuronBase.h"
#include "CActivationFunctionSigmoid.h"
#include "CActivationFunctionTanh.h"

CNeuronBase::CNeuronBase(): m_ActivationFunc( 0 ), m_Output( 0.0 ), m_Delta( 0.0 ), m_Error( 0.0 )
{
}

CNeuronBase::~CNeuronBase()
{
}

double CNeuronBase::getOutput()
{
	return m_Output;
}

void CNeuronBase::setOutput( double output )
{
	m_Output = m_ActivationFunc->Function( output );
}

double CNeuronBase::calculateDerivative( double value )
{
	return m_ActivationFunc->Derivative( value );
}

vector<Weight> & CNeuronBase::getWeights()
{
	return m_Weights;
}

double CNeuronBase::getError()
{
	return m_Error;
}

void CNeuronBase::setError( double error )
{
	m_Error = error;
}

//double CNeuronBase::getGradient()
//{
//	return m_Gradient;
//}
//
//void CNeuronBase::setGradient( double gradient )
//{
//	m_Gradient = gradient;
//}
