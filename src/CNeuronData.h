/*
 * CNeuronData.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef CNeuronData_H_
#define CNeuronData_H_

#include "INeuron.h"
#include "IActivationFunction.h"

class CNeuronData: public INeuron {
public:
	CNeuronData();
	~CNeuronData();

	double getOutput();
	void setOutput( double output );

	double calculateDerivative( double value );

	vector<Weight> & getWeights();

	double getDelta();
	void setDelta( double delta );

	double getError();
	void setError( double error );

private:

	double 			m_Output;
	vector<Weight> 	m_Weights;
};

#endif /* CNeuronData_H_ */
