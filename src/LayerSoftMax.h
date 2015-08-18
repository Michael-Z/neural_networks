/*
 * LayerSoftMax.h
 *
 *  Created on: Aug 18, 2015
 *      Author: vostanin
 */

#ifndef LAYERSOFTMAX_H_
#define LAYERSOFTMAX_H_

#include "ILayer.h"

class LayerSoftMax: public ILayer
{
public:
	LayerSoftMax( unsigned int neuronsCount );
	virtual ~LayerSoftMax();

	void initWeightsParams( double learningRate );
	unsigned int getNeuronsCount();
	INeuron * getNeuron( unsigned int index );
	void setWeights( unsigned int index, vector<Weight> & weights );

	double applyErrors( vector<double> & expected );
	double getErrorDerivative( uint32_t neuron_index );

private:
	void init( unsigned int neuronsCount );

	vector<INeuron*> m_neurons;

};

#endif /* LAYERSOFTMAX_H_ */
