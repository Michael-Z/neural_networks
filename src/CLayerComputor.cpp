/*
 * CHiddenLayer.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CLayerComputor.h"
#include "CNeuronSigmoid.h"

CLayerComputor::CLayerComputor( unsigned int neuronsCount )
{
	init( neuronsCount );
}

CLayerComputor::~CLayerComputor()
{
	size_t neurons_count = m_neurons.size();
	for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
	{
		delete m_neurons[neuron_i];
	}
}

void CLayerComputor::init( unsigned int neuronsCount )
{
	m_neurons.resize( neuronsCount );
	size_t neurons_count = m_neurons.size();
	for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
	{
		m_neurons[neuron_i] = new CNeuronSigmoid();
	}
}

unsigned int CLayerComputor::getNeuronsCount()
{
	return m_neurons.size();
}

void CLayerComputor::setWeights( unsigned int neuron_index, vector<double> & weights )
{
	m_neurons[neuron_index]->setWeights( weights );
}

INeuron * CLayerComputor::getNeuron( unsigned int index )
{
	return m_neurons[index];
}
