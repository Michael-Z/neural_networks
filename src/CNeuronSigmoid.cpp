/*
 * CNeuronSigmoid.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CNeuronSigmoid.h"
#include "CActivationFunctionSigmoid.h"
#include "CActivationFunctionTanh.h"

CNeuronSigmoid::CNeuronSigmoid()
{
	m_ActivationFunc = new CActivationFunctionSigmoid( 0.1 );
}

CNeuronSigmoid::~CNeuronSigmoid()
{
}
