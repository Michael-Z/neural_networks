/*
 * ILayer.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef ILAYER_H_
#define ILAYER_H_

#include <vector>
using namespace std;

#include "INeuron.h"

class ILayer
{
public:
	virtual void init( unsigned int neuronCount ) = 0;
	virtual unsigned int getNeuronsCount() = 0;
	virtual INeuron * getNeuron( unsigned int index ) = 0;
	virtual void setWeights( unsigned int index, vector<double> & weights ) = 0;
	virtual ~ILayer(){}
};

#endif /* ILAYER_H_ */
