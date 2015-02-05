/*
 * Neuron.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef NEURON_H_
#define NEURON_H_

#include <vector>
using namespace std;

class INeuron {
public:

	virtual double getOutput() = 0;
	virtual void setOutput( double output ) = 0;

	virtual void getWeights( vector<double> & weights ) = 0;
	virtual void setWeights( vector<double> & weights ) = 0;

	virtual double getDelta() = 0;
	virtual void setDelta( double output ) = 0;

	virtual double getError() = 0;
	virtual void setError( double output ) = 0;

	virtual ~INeuron(){}
};

#endif /* NEURON_H_ */
