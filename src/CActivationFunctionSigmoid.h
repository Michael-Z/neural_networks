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
	CActivationFunctionSigmoid( double alpha );
	virtual ~CActivationFunctionSigmoid();

	double Function( double value );
	double Derivative( double value );

private:

	 double m_Alpha;
};

#endif /* CActivationFunctionSigmoid_H_ */
