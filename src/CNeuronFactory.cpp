/*
 * CNeuronFactory.cpp
 *
 *  Created on: Oct 2, 2015
 *      Author: root
 */

#include "CNeuronFactory.h"
#include "CNeuronData.h"
#include "CNeuronSigmoid.h"
#include "assert.h"

CNeuronFactory::CNeuronFactory()
{
}

CNeuronFactory::~CNeuronFactory()
{
}

INeuron * CNeuronFactory::createNeuron( NeuronType type )
{
	INeuron * neuron = 0;
	switch( type )
	{
		case NEURON_DATA:
			neuron = new CNeuronData();
		break;
		case NEURON_SIGMOID:
			neuron = new CNeuronSigmoid();
		break;
		default:
			assert( false );
		break;
	}
	return neuron;
}

vector<INeuron *> CNeuronFactory::createNeurons( NeuronType type, uint32_t count )
{
	vector<INeuron *> neurons;
	for( uint32_t neuron_i = 0 ; neuron_i < count ; neuron_i++ )
	{
		INeuron * neuron = createNeuron( type );

		assert( 0 != neuron );

		neurons.push_back( neuron );
	}

	return neurons;
}














