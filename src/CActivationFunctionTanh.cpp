/*
 * CActivationFunctionSigmoid.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CActivationFunctionTanh.h"
#include "cmath"

CActivationFunctionTanh::CActivationFunctionTanh( double alpha ) : m_Alpha( alpha )
{
}

CActivationFunctionTanh::~CActivationFunctionTanh()
{
}

double CActivationFunctionTanh::Function( double value )
{
//	double result = ( exp( 2 * value / m_Alpha) - 1 ) / ( exp( 2 * value / m_Alpha ) + 1 );

	double result = ( 2.0 / ( 1 + exp( ( -1 ) * 2.0 * value / m_Alpha ) ) ) - 1.0;

	return result;
}

double CActivationFunctionTanh::Derivative( double value )
{
//	return 1 / ( cosh( value ) * cosh( value ) );
//	return ( value / m_Alpha ) * ( 1 - ( value / m_Alpha ) * ( value / m_Alpha ) );
	return 4.0 * ( exp( ( -1 ) * 2.0 * value * m_Alpha ) ) / ( m_Alpha * pow( exp( ( -1 ) * 2 * value * m_Alpha ), 2 ) + 2 * m_Alpha * exp( ( -1 ) * 2 / m_Alpha ) + m_Alpha );
}
