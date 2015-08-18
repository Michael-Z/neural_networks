/*
 * ILayer.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef ILAYER_H_
#define ILAYER_H_

#include <vector>
#include <stdint.h>
using namespace std;

#include "INeuron.h"

class ILayer
{
public:
	virtual void initWeightsParams( double learningRate ) = 0;
	virtual unsigned int getNeuronsCount() = 0;
	virtual INeuron * getNeuron( unsigned int index ) = 0;
	virtual void setWeights( unsigned int index, vector<Weight> & weights ) = 0;

	virtual double applyErrors( vector<double> & expected ) = 0;
	virtual double getErrorDerivative( uint32_t neuron_index ) = 0;

	virtual ~ILayer(){}
};

#endif /* ILAYER_H_ */
