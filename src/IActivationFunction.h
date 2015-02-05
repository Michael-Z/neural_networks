/*
 * IActivationLayer.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef IACTIVATIONLAYER_H_
#define IACTIVATIONLAYER_H_

class IActivationFunction {
public:
	virtual double Activate( double value ) = 0;
	virtual ~IActivationFunction(){}
};

#endif /* IACTIVATIONLAYER_H_ */
