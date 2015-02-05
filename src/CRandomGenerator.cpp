/*
 * CRandomGenerator.cpp
 *
 *  Created on: Jan 27, 2015
 *      Author: vostanin
 */

#include "CRandomGenerator.h"
#include <ctime>
#include <cstdlib>

CRandomGenerator::CRandomGenerator( double range_low, double range_high, double range_precision ):
					m_Low( range_low ), m_High( range_high ), m_Precision( range_precision )
{
	// TODO Auto-generated constructor stub
	srand(time(NULL));
}

CRandomGenerator::~CRandomGenerator()
{
	// TODO Auto-generated destructor stub
}

double CRandomGenerator::getValue()
{
	return ((int)rand()%(int)(1.0/m_Precision))/(float) (1.0/m_Precision) - 0.5;
}

void CRandomGenerator::fillVector( vector<double> & values )
{
	size_t values_count = values.size();

	for( size_t value_i = 0 ; value_i < values_count ; value_i++ )
	{
		values[value_i] = getValue();
	}
}
