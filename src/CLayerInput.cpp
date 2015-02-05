/*
 * CHiddenLayer.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CLayerInput.h"
#include "CNeuronData.h"

CLayerInput::CLayerInput( unsigned int neuronsCount )
{
	init( neuronsCount );
}

CLayerInput::~CLayerInput()
{
	size_t neurons_count = m_neurons.size();
	for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
	{
		delete m_neurons[neuron_i];
	}
}

void CLayerInput::init( unsigned int neuronsCount )
{
	m_neurons.resize( neuronsCount );
	size_t neurons_count = m_neurons.size();
	for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
	{
		m_neurons[neuron_i] = new CNeuronData();
	}
}

unsigned int CLayerInput::getNeuronsCount()
{
	return m_neurons.size();
}

void CLayerInput::setWeights( unsigned int neuron_index, vector<double> & weights )
{
	m_neurons[neuron_index]->setWeights( weights );
}

INeuron * CLayerInput::getNeuron( unsigned int index )
{
	return m_neurons[index];
}
