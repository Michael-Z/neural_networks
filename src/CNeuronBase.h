/*
 * CNeuronSigmoid.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef CNeuronBase_H_
#define CNeuronBase_H_

#include "INeuron.h"
#include "IActivationFunction.h"

class CNeuronBase: public INeuron
{
public:
	CNeuronBase();
	~CNeuronBase();

	virtual double getOutput();
	virtual void setOutput( double output );

	virtual double calculateDerivative( double value );

	virtual vector<Weight> & getWeights();

	virtual double getError();
	virtual void setError( double error );

protected:

	IActivationFunction * m_ActivationFunc;

private:

	double 			m_Output;
	vector<Weight> 	m_Weights;
	double 			m_Delta;
	double 			m_Error;
};

#endif /* CNeuronBase_H_ */
