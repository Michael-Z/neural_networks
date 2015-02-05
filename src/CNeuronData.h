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

	void getWeights( vector<double> & weights );
	void setWeights( vector<double> & weights );

	double getDelta();
	void setDelta( double delta );

	double getError();
	void setError( double error );

private:

	double 			m_Output;
	vector<double> 	m_Weights;
	double 			m_Delta;
	double 			m_Error;
};

#endif /* CNeuronData_H_ */
