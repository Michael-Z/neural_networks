/*
 * CLayersConfiguration.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef CLayersConfiguration_H_
#define CLayersConfiguration_H_

#include "CLayerMatrix.h"
#include <vector>

class CLayersConfiguration
{
public:
	CLayersConfiguration( unsigned int inputNeuronsCount, unsigned int outputNeuronsCount, vector<size_t> & hiddenLayers );
	CLayersConfiguration( vector<size_t> & layers );
	~CLayersConfiguration();

	CLayersConfiguration & getLayers( vector<ILayer*> & layers );
	CLayersConfiguration & getLayers( vector<CLayerMatrix*> & layers );

private:
	vector<ILayer*> m_layers;
	vector<CLayerMatrix*> m_layersMatrix;
};

#endif /* CLayersConfiguration_H_ */
