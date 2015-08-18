/*
 * Weight.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: vostanin
 */

#include "Weight.h"

Weight::Weight() : m_Value( 0.0 ), m_Gradient( 0.0 ), m_LearningRate( 0.0 )
{
}

Weight::Weight( double value ) : m_Value( value ), m_Gradient( 0.0 ), m_LearningRate( 0.0 )
{

}

Weight::~Weight()
{
}

void Weight::value( double value )
{
	m_Value = value;
}

double Weight::value() const
{
	return m_Value;
}

void Weight::gradient( double gradient )
{
	m_Gradient = gradient;
}

double Weight::gradient() const
{
	return m_Gradient;
}

void Weight::learningRate( double rate )
{
	m_LearningRate = rate;
}

double Weight::learningRate() const
{
	return m_LearningRate;
}







