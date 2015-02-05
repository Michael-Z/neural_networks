/*
 * CActivationFunctionSigmoid.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef CActivationFunctionSigmoid_H_
#define CActivationFunctionSigmoid_H_

#include "IActivationFunction.h"

class CActivationFunctionSigmoid: public IActivationFunction {
public:
	CActivationFunctionSigmoid();
	virtual ~CActivationFunctionSigmoid();

	virtual double Activate( double value );
};

#endif /* CActivationFunctionSigmoid_H_ */
