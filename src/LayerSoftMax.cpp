/*
 * LayerSoftMax.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: vostanin
 */

#include "LayerSoftMax.h"
#include "CNeuronSigmoid.h"
#include <cassert>

LayerSoftMax::LayerSoftMax( unsigned int neuronsCount )
{
}

LayerSoftMax::~LayerSoftMax()
{
}

void LayerSoftMax::init( unsigned int neuronsCount )
{
	m_neurons.resize( neuronsCount );
	size_t neurons_count = m_neurons.size();
	for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
	{
		m_neurons[neuron_i] = new CNeuronSigmoid();
	}
}

void LayerSoftMax::initWeightsParams( double learningRate )
{
	size_t neurons_count = m_neurons.size();
	for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
	{
		vector<Weight> & weights = m_neurons[neuron_i]->getWeights();
		size_t weightsCount = weights.size();
		for( size_t weight_i = 0 ; weight_i < weightsCount ; weight_i++ )
		{
			weights[weight_i].learningRate( learningRate );
		}
	}
}

unsigned int LayerSoftMax::getNeuronsCount()
{
	return m_neurons.size();
}

INeuron * LayerSoftMax::getNeuron( unsigned int index )
{
	return m_neurons[index];
}

void LayerSoftMax::setWeights( unsigned int neuron_index, vector<Weight> & weights )
{
	m_neurons[neuron_index]->getWeights().assign( weights.begin(), weights.end() );
}

double LayerSoftMax::applyErrors( vector<double> & expected )
{
	uint32_t actualSize = m_neurons.size();
	uint32_t expectedSize = expected.size();
	if( actualSize != expectedSize )
	{
		assert( false );
	}

	for( uint32_t neuron_i = 0 ; neuron_i < actualSize ; neuron_i++ )
	{
		double error = m_neurons[neuron_i]->getOutput() - expected[neuron_i];
		m_neurons[neuron_i]->setError( error );
	}
}

double LayerSoftMax::getErrorDerivative( uint32_t neuron_index )
{
	return m_neurons[neuron_index]->getError();
}








