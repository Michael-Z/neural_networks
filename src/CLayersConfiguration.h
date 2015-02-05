/*
 * CLayersConfiguration.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef CLayersConfiguration_H_
#define CLayersConfiguration_H_

#include "CLayerComputor.h"
#include <vector>
using namespace std;

class CLayersConfiguration
{
public:
	CLayersConfiguration( unsigned int inputNeuronsCount, unsigned int outputNeuronsCount, vector<size_t> & hiddenLayers );
	~CLayersConfiguration();

	CLayersConfiguration & getLayers( vector<ILayer*> & layers );

private:
	vector<ILayer*> m_layers;
};

#endif /* CLayersConfiguration_H_ */
