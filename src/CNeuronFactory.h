/*
 * CNeuronFactory.h
 *
 *  Created on: Oct 2, 2015
 *      Author: root
 */

#ifndef CNEURONFACTORY_H_
#define CNEURONFACTORY_H_

#include "INeuron.h"
#include "CSinglton.h"
#include <stdint.h>

class CNeuronFactory : public Singlton<CNeuronFactory>
{
public:
	CNeuronFactory();
	virtual ~CNeuronFactory();

	INeuron * createNeuron( NeuronType type );
	vector<INeuron *> createNeurons( NeuronType type, uint32_t count );
};

#endif /* CNEURONFACTORY_H_ */
