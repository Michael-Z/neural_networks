/*
 * CHiddenLayer.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CLayerInput.h"
#include "CNeuronData.h"
#include "CNeuronFactory.h"

CLayerInput::CLayerInput( unsigned int neuronsCount, NeuronType neuronType )
{
	init( neuronsCount, neuronType );
}

CLayerInput::~CLayerInput()
{
	size_t neurons_count = m_neurons.size();
	for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
	{
		delete m_neurons[neuron_i];
	}
}

void CLayerInput::init( unsigned int neuronsCount, NeuronType neuronType )
{
	m_neurons.resize( neuronsCount );
	size_t neurons_count = m_neurons.size();
	for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
	{
		m_neurons[neuron_i] = CNeuronFactory::Instance().createNeuron( NEURON_DATA );
	}
}

void CLayerInput::initWeightsParams( double learningRate )
{
	//empty
}

unsigned int CLayerInput::getNeuronsCount()
{
	return m_neurons.size();
}

void CLayerInput::setWeights( unsigned int neuron_index, vector<Weight> & weights )
{
	//empty
}

INeuron * CLayerInput::getNeuron( unsigned int index )
{
	return m_neurons[index];
}

double CLayerInput::applyErrors( vector<double> & expected )
{
	return 0.0;
}

double CLayerInput::getErrorDerivative( uint32_t neuron_index )
{
	return 0.0;
}









