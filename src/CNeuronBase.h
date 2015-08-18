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

class CNeuronBase: public INeuron {
public:
	CNeuronBase();
	~CNeuronBase();

	double getOutput();
	void setOutput( double output );

	double calculateDerivative( double value );


	vector<Weight> & getWeights();

	double getDelta();
	void setDelta( double delta );

	double getError();
	void setError( double error );

	double getGradient();
	void setGradient( double gradient );


	IActivationFunction * activationFunc;

private:

	double 			m_Output;
	vector<Weight> 	m_Weights;
	double 			m_Delta;
	double 			m_Error;
	double 			m_Gradient;
};

#endif /* CNeuronBase_H_ */
