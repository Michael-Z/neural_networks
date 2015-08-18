/*
 * CneuronSoftmax.h
 *
 *  Created on: Aug 18, 2015
 *      Author: vostanin
 */

#ifndef CNEURONSOFTMAX_H_
#define CNEURONSOFTMAX_H_

#include "CNeuronBase.h"

class CNeuronSoftmax : public CNeuronBase
{
public:
	CNeuronSoftmax();
	virtual ~CNeuronSoftmax();

	virtual void setOutput( double output );

	virtual double calculateDerivative( double value );

private:

	double m_Output;

};

#endif /* CNEURONSOFTMAX_H_ */
