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
	activationFunc = new CActivationFunctionTanh( 1 );
}

CNeuronTanh::~CNeuronTanh()
{
}
