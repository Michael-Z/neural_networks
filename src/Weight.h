/*
 * Weight.h
 *
 *  Created on: Aug 18, 2015
 *      Author: vostanin
 */

#ifndef WEIGHT_H_
#define WEIGHT_H_

class Weight
{
public:
	Weight();
	Weight( double value );
	virtual ~Weight();

	void value( double value );
	double value() const;

	void gradient( double value );
	double gradient() const;

	void learningRate( double rate );
	double learningRate() const;

private:

	double m_Value;
	double m_Gradient;
	double m_LearningRate;

};

#endif /* WEIGHT_H_ */
