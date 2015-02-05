/*
 * CLayersConfiguration.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CLayersConfiguration.h"
#include "CLayerInput.h"
#include "CLayerComputor.h"

CLayersConfiguration::CLayersConfiguration( unsigned int inputNeuronsCount, unsigned int outputNeuronsCount, vector<size_t> & hiddenLayers )
{
	ILayer * layer = new CLayerInput( inputNeuronsCount );
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
