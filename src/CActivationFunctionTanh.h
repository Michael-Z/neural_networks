/*
 * CActivationFunctionTanh.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef CActivationFunctionTanh_H_
#define CActivationFunctionTanh_H_

#include "IActivationFunction.h"

class CActivationFunctionTanh: public IActivationFunction {
public:
	CActivationFunctionTanh( double alpha );
	virtual ~CActivationFunctionTanh();

	double Function( double value );
	double Derivative( double value );

private:

	 double m_Alpha;
};

#endif /* CActivationFunctionTanh_H_ */
