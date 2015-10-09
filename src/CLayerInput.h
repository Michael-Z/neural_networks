/*
 * CHiddenLayer.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef CLayerInput_H_
#define CLayerInput_H_

#include "ILayer.h"
#include "INeuron.h"
#include <vector>
using namespace std;

class CLayerInput: public ILayer
{
public:
	CLayerInput( unsigned int neuronsCount, NeuronType neuronType );
	virtual ~CLayerInput();
	virtual void initWeightsParams( double learningRate );
	virtual void setWeights( unsigned int neuron_index, vector<Weight> & weights );
	virtual unsigned int getNeuronsCount();
	virtual INeuron * getNeuron( unsigned int index );
	virtual double applyErrors( vector<double> & expected );
	virtual double getErrorDerivative( uint32_t neuron_index );
private:
	void init( unsigned int neuronsCount, NeuronType neuronType );
private:
	vector<INeuron*> m_neurons;
};

#endif /* CLayerInput_H_ */
