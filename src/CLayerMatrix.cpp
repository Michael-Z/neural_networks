/*
 * CLayerMatrix.cpp
 *
 *  Created on: Oct 2, 2015
 *      Author: root
 */

#include "CLayerMatrix.h"
#include "CRandomGenerator.h"
#include <vector>
#include <string.h>
using namespace std;

CLayerMatrix::CLayerMatrix( uint32_t inputCount, uint32_t outputCount ) :
				m_Weights( outputCount, inputCount + 1, 0.5 ), m_WeightsGradients( outputCount, inputCount + 1, 0.0 ), m_LearningRate( outputCount, inputCount + 1, 1 ), m_NextLayer( 0 ), m_PrevLayer( 0 )
{
//	uint32_t elementsCount = inputCount * outputCount;
//	std::vector<double> weights( elementsCount );
//	CRandomGenerator random( -0.5, 0.5, 0.00001 );
//	random.fillVector( weights );
//	boost::numeric::ublas::matrix<double>::array_type & matrixData = m_Weights.data();
//	memcpy( &matrixData[0], &weights[0], elementsCount * sizeof( double ) );
	m_inputNeurons.resize( inputCount, NULL );
	m_outputNeurons.resize( outputCount, NULL );
}

CLayerMatrix::~CLayerMatrix()
{
}

//void CLayerMatrix::initWeightsParams( double learningRate );
uint32_t CLayerMatrix::getInputNeuronsCount()
{
	return m_inputNeurons.size();
}

uint32_t CLayerMatrix::getOutputNeuronsCount()
{
	return m_outputNeurons.size();
}

INeuron * CLayerMatrix::getInputNeuron( uint32_t index )
{
	return m_inputNeurons[index];
}

INeuron * CLayerMatrix::getOutputNeuron( uint32_t index )
{
	return m_outputNeurons[index];
}

void CLayerMatrix::setInputNeuron( uint32_t index, INeuron * neuron )
{
	m_inputNeurons[index] = neuron;
}

void CLayerMatrix::setOuputNeuron( uint32_t index, INeuron * neuron )
{
	m_outputNeurons[index] = neuron;
}

void CLayerMatrix::setInputNeurons( vector<INeuron *> & neurons )
{
	assert( m_inputNeurons.size() == neurons.size() );

	m_inputNeurons = neurons;
}

void CLayerMatrix::setOutputNeurons( vector<INeuron *> & neurons )
{
	assert( m_outputNeurons.size() == neurons.size() );

	m_outputNeurons = neurons;
}

void CLayerMatrix::setNextLayer( CLayerMatrix * layer )
{
	m_NextLayer = layer;
}

void CLayerMatrix::setPrevLayer( CLayerMatrix * layer )
{
	m_PrevLayer = layer;
}

double CLayerMatrix::forward( const boost::numeric::ublas::vector<double> & input, const boost::numeric::ublas::vector<double> & expected )
{
	uint32_t weightsSize1 = m_Weights.size1();
	uint32_t weightsSize2 = m_Weights.size2();
	uint32_t inputSize = input.size();
	assert( weightsSize2 == inputSize );
//	boost::numeric::ublas::vector<double> VecSum = boost::numeric::ublas::prod( m_Weights, input );

	boost::numeric::ublas::vector<double> VecSum( weightsSize1, 0.0 );
	double v = 0.0;
	for( uint32_t weightsRow_i = 0 ; weightsRow_i < weightsSize1 ; weightsRow_i ++ )
	{
		for( uint32_t weightsColumn_i = 0 ; weightsColumn_i < weightsSize2 ; weightsColumn_i ++ )
		{
			double weight = m_Weights.at_element( weightsRow_i, weightsColumn_i );
			double in = input[weightsColumn_i];
			v = VecSum[weightsRow_i];
			VecSum[weightsRow_i] += weight * in;
			v = VecSum[weightsRow_i];

		}
	}

	uint32_t outputNeuronsCount = getOutputNeuronsCount();
	assert( VecSum.size() == m_Weights.size1() );
	assert( VecSum.size() == outputNeuronsCount );



	for( uint32_t neuron_i = 0 ; neuron_i < outputNeuronsCount ; neuron_i++ )
	{
		INeuron * neuron = getOutputNeuron( neuron_i );
		double output = VecSum[neuron_i];
		neuron->setOutput( output );
	}

	if( NULL == m_NextLayer )
	{
		return getError( expected );
	}

	boost::numeric::ublas::vector<double> nextInput( outputNeuronsCount + 1, 1.0 /*bias*/ );

	for( uint32_t neuron_i = 0 ; neuron_i < outputNeuronsCount ; neuron_i++ )
	{
		INeuron * neuron = getOutputNeuron( neuron_i );
		nextInput[neuron_i + 1] = neuron->getOutput();
	}

	return m_NextLayer->forward( nextInput, expected );
}

double CLayerMatrix::getError( const boost::numeric::ublas::vector<double> & expected )
{
	double wholeError = 0.0;

	uint32_t outputNeuronsCount = getOutputNeuronsCount();
	for( uint32_t neuron_i = 0 ; neuron_i < outputNeuronsCount ; neuron_i++ )
	{
		INeuron * neuron = getOutputNeuron( neuron_i );

		double expect = expected[neuron_i];
		double output = neuron->getOutput();
//		double error = ( 1.0 ) * expect * log2( output ) - ( 1.0 - expect ) * log2( 1.0 - output );
		double error = ( expect - output );
		neuron->setError( error );

		wholeError += abs( error * error );
	}

	return 1 / 2.0 * wholeError;
}

//void CLayerMatrix::setWeights( unsigned int index, vector<Weight> & weights );
//
//double CLayerMatrix::applyErrors( vector<double> & expected );
//double CLayerMatrix::getErrorDerivative( uint32_t neuron_index )
//{
//
//}
