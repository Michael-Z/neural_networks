/*
 * CNeuronSigmoid.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CNeuronTanh.h"
#include "CActivationFunctionSigmoid.h"
#include "CActivationFunctionTanh.h"

CNeuronTanh::CNeuronTanh()
{
	m_ActivationFunc = new CActivationFunctionTanh( 1 );
}

CNeuronTanh::~CNeuronTanh()
{
}
