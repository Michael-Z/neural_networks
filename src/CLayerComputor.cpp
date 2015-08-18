/*
 * CHiddenLayer.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CLayerComputor.h"
#include "CNeuronSigmoid.h"
#include "CNeuronTanh.h"
#include <cstdio>
#include <cmath>
#include <cassert>

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

void CLayerComputor::initWeightsParams( double learningRate )
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

unsigned int CLayerComputor::getNeuronsCount()
{
	return m_neurons.size();
}

void CLayerComputor::setWeights( unsigned int neuron_index, vector<Weight> & weights )
{
	m_neurons[neuron_index]->getWeights().assign( weights.begin(), weights.end() );
}

INeuron * CLayerComputor::getNeuron( unsigned int index )
{
	return m_neurons[index];
}

double CLayerComputor::applyErrors( vector<double> & expected )
{
	uint32_t actualSize = m_neurons.size();
	uint32_t expectedSize = expected.size();
	if( actualSize != expectedSize )
	{
		assert( false );
	}

	double errorSum = 0.0;

	for( uint32_t neuron_i = 0 ; neuron_i < actualSize ; neuron_i++ )
	{
		double error = m_neurons[neuron_i]->getOutput() - expected[neuron_i];
		m_neurons[neuron_i]->setError( error );

		errorSum += abs( error );
	}
	return errorSum;
}

double CLayerComputor::getErrorDerivative( uint32_t neuron_index )
{
	double error = m_neurons[neuron_index]->getError();
	return 2 * error * error;
}







