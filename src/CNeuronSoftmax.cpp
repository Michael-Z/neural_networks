/*
 * CneuronSoftmax.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: vostanin
 */

#include "CNeuronSoftmax.h"

CNeuronSoftmax::CNeuronSoftmax() : m_Output( 0.0 )
{
}

CNeuronSoftmax::~CNeuronSoftmax()
{
}

void CNeuronSoftmax::setOutput( double output )
{
	m_Output = output;
}

double CNeuronSoftmax::calculateDerivative( double value )
{
	return value * ( 1 - value );
}









