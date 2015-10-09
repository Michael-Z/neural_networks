/*
 * CLayersConfiguration.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CLayersConfiguration.h"
#include "CLayerInput.h"
#include "CLayerComputor.h"
#include "CLayerMatrix.h"
#include "CNeuronFactory.h"
#include <numeric>

CLayersConfiguration::CLayersConfiguration( unsigned int inputNeuronsCount, unsigned int outputNeuronsCount, vector<size_t> & hiddenLayers )
{
	ILayer * layer = new CLayerInput( inputNeuronsCount, NEURON_DATA );
	m_layers.push_back( layer );//Input layer
	size_t hiddenLayersCount  = hiddenLayers.size();
	for( size_t hiddenLayer_i = 0 ; hiddenLayer_i < hiddenLayersCount ; hiddenLayer_i++ )
	{
		layer = new CLayerComputor( hiddenLayers[hiddenLayer_i] );
		m_layers.push_back( layer );//Hidden layer
	}
	layer = new CLayerComputor( outputNeuronsCount );
	m_layers.push_back( layer );//Output layer
}

CLayersConfiguration::CLayersConfiguration( vector<size_t> & layers )
{
	size_t layersCount  = layers.size();
	const uint32_t minLayersCount = 2;

	assert( layersCount >= minLayersCount );

	uint32_t inputNeuronsCount = layers[0];

	vector<INeuron *> inputNeurons;
	vector<INeuron *> outputNeurons = CNeuronFactory::Instance().createNeurons( NEURON_DATA, inputNeuronsCount );

//	uint32_t weightsCount = 0.0;
//
//	for( size_t layer_i = 0 ; layer_i < ( layersCount - 1 ); layer_i++ )
//	{
//		uint32_t inputNeuronsCount = layers[layer_i];
//		uint32_t outputNeuronsCount = layers[layer_i + 1];
//
//		weightsCount += ( inputNeurons + 1 /*bias*/ ) * outputNeurons;
//	}
//
//	m_X = lbfgs_malloc( m_weightsCount );

	for( size_t layer_i = 0 ; layer_i < ( layersCount - 1 ); layer_i++ )
	{
		inputNeuronsCount = layers[layer_i];
		uint32_t outputNeuronsCount = layers[layer_i + 1];

		NeuronType outputNeuronType = NEURON_SIGMOID;

		inputNeurons = outputNeurons;

		outputNeurons = CNeuronFactory::Instance().createNeurons( outputNeuronType, outputNeuronsCount );

		CLayerMatrix * layer = new CLayerMatrix( inputNeuronsCount, outputNeuronsCount );

		layer->setInputNeurons( inputNeurons );
		layer->setOutputNeurons( outputNeurons );
		m_layersMatrix.push_back( layer );
		if( layer_i > 0 )
		{
			m_layersMatrix[layer_i - 1]->setNextLayer( layer );
			layer->setPrevLayer( m_layersMatrix[layer_i - 1] );
		}
	}

//	m_X = lbfgs_malloc( m_weightsCount );
//
//	lbfgs_parameter_init(&m_LbfgsParam);

}

CLayersConfiguration::~CLayersConfiguration()
{
	size_t layers_count = m_layers.size();
	for( size_t layer_i = 0 ; layer_i < layers_count ; layer_i++ )
	{
		delete m_layers[layer_i];
	}
	m_layers.clear();
}

CLayersConfiguration & CLayersConfiguration::getLayers( vector<ILayer*> & layers )
{
	layers.assign( m_layers.begin(), m_layers.end() );

	return *this;
}

CLayersConfiguration & CLayersConfiguration::getLayers( vector<CLayerMatrix*> & layers )
{
	layers.assign( m_layersMatrix.begin(), m_layersMatrix.end() );

	return *this;
}



