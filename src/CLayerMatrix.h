/*
 * CLayerMatrix.h
 *
 *  Created on: Oct 2, 2015
 *      Author: root
 */

#ifndef CLAYERMATRIX_H_
#define CLAYERMATRIX_H_

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "ILayer.h"
#include <stdint.h>

class CLayerMatrix
{
public:
	CLayerMatrix( uint32_t inputCount, uint32_t outputCount );
	virtual ~CLayerMatrix();

	void setInputNeuron( uint32_t index, INeuron * neuron );
	void setOuputNeuron( uint32_t index, INeuron * neuron );

	void setInputNeurons( vector<INeuron *> & neurons );
	void setOutputNeurons( vector<INeuron *> & neurons );

	void setNextLayer( CLayerMatrix * layer );
	void setPrevLayer( CLayerMatrix * layer );

//	void initWeightsParams( double learningRate );
	uint32_t getInputNeuronsCount();
	uint32_t getOutputNeuronsCount();
	INeuron * getInputNeuron( unsigned int index );
	INeuron * getOutputNeuron( unsigned int index );

	boost::numeric::ublas::matrix<double> & getGradients(){ return m_WeightsGradients; }
	boost::numeric::ublas::matrix<double> & getWeights(){ return m_Weights; }
	boost::numeric::ublas::matrix<double> & getLearningRate(){ return m_LearningRate; }

	double forward( const boost::numeric::ublas::vector<double> & input, const boost::numeric::ublas::vector<double> & expected );
	double getError( const boost::numeric::ublas::vector<double> & expected );
//	void setWeights( unsigned int index, vector<Weight> & weights );
//
//	double applyErrors( vector<double> & expected );
//	double getErrorDerivative( uint32_t neuron_index );

private:

	boost::numeric::ublas::matrix<double> m_Weights;
	boost::numeric::ublas::matrix<double> m_WeightsGradients;
	boost::numeric::ublas::matrix<double> m_LearningRate;

	vector<INeuron*> m_outputNeurons;
	vector<INeuron*> m_inputNeurons;

	CLayerMatrix * m_NextLayer;
	CLayerMatrix * m_PrevLayer;
};

#endif /* CLAYERMATRIX_H_ */
