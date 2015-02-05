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
	CLayerInput( unsigned int neuronsCount );
	virtual ~CLayerInput();
	void setWeights( unsigned int neuron_index, vector<double> & weights );
	void setValues( vector<double> & values );
	void calculateWeights( ILayer * prev_layer );
	void getWeights( vector<double> & weights );
	unsigned int getNeuronsCount();
	INeuron * getNeuron( unsigned int index );
private:
	void init( unsigned int neuronsCount );
private:
	vector<INeuron*> m_neurons;
};

#endif /* CLayerInput_H_ */
