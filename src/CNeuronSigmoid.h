/*
 * CNeuronSigmoid.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef CNeuronSigmoid_H_
#define CNeuronSigmoid_H_

#include "INeuron.h"
#include "IActivationFunction.h"

class CNeuronSigmoid: public INeuron {
public:
	CNeuronSigmoid();
	~CNeuronSigmoid();

	double getOutput();
	void setOutput( double output );

	void getWeights( vector<double> & weights );
	void setWeights( vector<double> & weights );

	double getDelta();
	void setDelta( double delta );

	double getError();
	void setError( double error );

private:
	IActivationFunction * activationFunc;

	double 			m_Output;
	vector<double> 	m_Weights;
	double 			m_Delta;
	double 			m_Error;
};

#endif /* CNeuronSigmoid_H_ */
