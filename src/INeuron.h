/*
 * Neuron.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef NEURON_H_
#define NEURON_H_

#include <vector>
#include "Weight.h"
using namespace std;

class INeuron
{
public:

	virtual double getOutput() = 0;
	virtual void setOutput( double output ) = 0;

	virtual double calculateDerivative( double value ) = 0;

	virtual vector<Weight> & getWeights() = 0;

	virtual double getError() = 0;
	virtual void setError( double output ) = 0;

	virtual ~INeuron(){}
};

#endif /* NEURON_H_ */
