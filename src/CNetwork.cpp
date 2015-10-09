/*
 * CNetwork.cpp
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#include "CNetwork.h"
#include "CRandomGenerator.h"
#include <cmath>
#include <cstdio>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <boost/assign/std/vector.hpp>

#define BIASOUTPUT 1   //output for bias. It's always 1.

CNetwork::CNetwork( CLayersConfiguration & sequence, double learningRateStart, double momentRate ):
		  m_fromFile( false ), m_LearningRate( learningRateStart ), m_MomentRate( momentRate ), m_LayersConfiguration( &sequence ), m_epochStateCallback( NULL ), m_weightsCount( 0 )
{
	createNetwork( 0 );
//	createNetwork();

	m_X = lbfgs_malloc( m_weightsCount );

	lbfgs_parameter_init(&m_LbfgsParam);
}

CNetwork::CNetwork( string & filename ): m_fromFile( true ), m_LearningRate( 1.0 ), m_MomentRate( 0 ), m_LayersConfiguration( NULL ), m_epochStateCallback( NULL )
{
	load( filename );
}

CNetwork::~CNetwork()
{
	if( m_fromFile )
	{
		delete m_LayersConfiguration;
	}
}

void CNetwork::createNetwork()
{
	vector<ILayer*> layers;
	m_LayersConfiguration->getLayers( layers );

	size_t layers_count = layers.size();

	CRandomGenerator random( -0.5, 0.5, 0.00001 );

	for( size_t layer_i = 1 ; layer_i < layers_count ; layer_i++ )
	{
		ILayer * currlayer = layers[layer_i];
		ILayer * prevlayer = layers[layer_i - 1];
		unsigned int neurons_count = currlayer->getNeuronsCount();
		for( unsigned int neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
		{
			vector<Weight> weights( prevlayer->getNeuronsCount() + 1, 0.5 );
			random.fillVector( weights );

			currlayer->setWeights( neuron_i, weights );
			m_weightsCount += weights.size();
		}
		currlayer->initWeightsParams( m_LearningRate );
	}
}

void CNetwork::createNetwork( int32_t reserved )
{
	vector<CLayerMatrix*> layers;
	m_LayersConfiguration->getLayers( layers );

	size_t layers_count = layers.size();

	CRandomGenerator random( -0.5, 0.5, 0.00001 );

	for( size_t layer_i = 0 ; layer_i < layers_count ; layer_i++ )
	{
		CLayerMatrix * layer = layers[layer_i];
		uint32_t size1 = layer->getWeights().size1();
		uint32_t size2 = layer->getWeights().size2();
		m_weightsCount += size1 * size2;
	}
}

void CNetwork::printInputLayer( CNetwork & network )
{
	vector<CLayerMatrix*> layers;
	network.m_LayersConfiguration->getLayers( layers );
	uint32_t layersCount = layers.size();

	for( uint32_t layer_i = 0 ; layer_i < layersCount ; layer_i++ )
	{
		CLayerMatrix * layer = layers[layer_i];
		boost::numeric::ublas::matrix<double> & weights = layer->getWeights();
		std::for_each( weights.data().begin(), weights.data().end(), [&]( double & w ) {

			printf( "weight[%d]=%f\n", layer_i, w ); fflush( stdout );
		} );
	}
}

double CNetwork::forward( vector<double> & inputData, vector<double> & outputData )
{
	vector<ILayer*> layers;
	m_LayersConfiguration->getLayers( layers );

	size_t layers_count = layers.size();
	size_t input_count = inputData.size();
	size_t output_count = outputData.size();

	if( input_count != layers[0]->getNeuronsCount() )
	{
		return 0.0;
	}
	if( output_count != layers[layers_count - 1]->getNeuronsCount() )
	{
		return 0.0;
	}

	for( size_t input_i = 0 ; input_i < input_count ; input_i++ )
	{
		layers[0]->getNeuron(input_i)->setOutput( inputData[input_i] );
	}

	for( size_t layer_i = 1 ; layer_i < layers_count ; layer_i++ )
	{
		ILayer * layer = layers[layer_i];
		ILayer * prevLayer = layers[layer_i - 1];
		size_t neurons_count = layer->getNeuronsCount();
		size_t prev_layer_neurons_count = prevLayer->getNeuronsCount();
		for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
		{
			double multiplied_weight_sum = 0.0;
			vector<Weight> & weights = layer->getNeuron( neuron_i )->getWeights();
			uint32_t weightsCount = weights.size();
			for( size_t prev_layer_neuron_i = 0 ; prev_layer_neuron_i <= prev_layer_neurons_count ; prev_layer_neuron_i++ )
			{
				double multiplied_weight = 0.0;

				if( prev_layer_neuron_i == 0 )
				{
					multiplied_weight = weights[prev_layer_neuron_i].value() * BIASOUTPUT;
				}
				else
				{
					double output = prevLayer->getNeuron( prev_layer_neuron_i - 1 )->getOutput();
					multiplied_weight = weights[prev_layer_neuron_i].value() * output;
				}

				multiplied_weight_sum += multiplied_weight;
			}

			layer->getNeuron( neuron_i )->setOutput( multiplied_weight_sum );
		}
	}

	ILayer * last_layer = layers[layers_count - 1];

	double sum_square_error = last_layer->applyErrors( outputData );

	double mean_square_error = sum_square_error / (double)last_layer->getNeuronsCount();

	return mean_square_error;

}

double CNetwork::forward( vector<double> & inputData, vector<double> & outputData, uint32_t reserved )
{
	vector<CLayerMatrix*> layers;
	m_LayersConfiguration->getLayers( layers );

	size_t layersCount = layers.size();
	size_t inputCount = inputData.size();
	size_t outputCount = outputData.size();

	if( inputCount != layers[0]->getInputNeuronsCount() )
	{
		return 0.0;
	}
	if( outputCount != layers[layersCount - 1]->getOutputNeuronsCount() )
	{
		return 0.0;
	}

	for( size_t input_i = 0 ; input_i < inputCount ; input_i++ )
	{
		layers[0]->getInputNeuron(input_i)->setOutput( inputData[input_i] );
	}

	CLayerMatrix * layer = layers[0];
	boost::numeric::ublas::vector<double> boostInput( inputCount + 1, 1.0/*bias*/);
	boost::numeric::ublas::vector<double> boostExpected( outputCount );
	std::copy( inputData.begin()/*bias*/, inputData.end(), boostInput.begin() + 1 );
	std::copy( outputData.begin(), outputData.end(), boostExpected.begin() );

	double wholeError = layer->forward( boostInput, boostExpected );

	double wholeErrorMean = wholeError / outputCount;

	return wholeErrorMean;

}

//vector<double> findLocalMin( vector<double> & /*in out*/ vars, uint32_t varIndex, funcGrad_t funcGrad, uint32_t & iterationCount )
//{
//	double learningRateAccuracy = 1e-7;
//	double derivativeAccuracy = 1e-7;
//	vector<double> oldFuncDerivatives = funcGrad( vars );
//	double oldFuncVarDerivative = oldFuncDerivatives[varIndex];
//
//	double learningRate = 1;
//	iterationCount = 0;
//	do
//	{
//		double funcAntiGradValue = ( -1 ) * oldFuncVarDerivative;
//
//		vars[varIndex] = vars[varIndex] + funcAntiGradValue * learningRate;
//		vector<double> newFuncDerivates = funcGrad( vars );
//		double newFuncVarDerivate = newFuncDerivates[varIndex];
//
//		if( newFuncVarDerivate * oldFuncVarDerivative < 0 )
//		{
//			learningRate /= 3.0;
//		}
//		else
//		{
//			learningRate *= 1.5;
//		}
//
//		bool exitCondition = learningRate < learningRateAccuracy || abs( newFuncVarDerivate ) < derivativeAccuracy;
//		if( true == exitCondition )
//		{
//			break;
//		}
//
//		oldFuncVarDerivative = newFuncVarDerivate;
//		iterationCount++;
//
//	}while( true );
//
//	return vars;
//}

double getGradientsSum( vector<double> & varsDerivatives )
{
	return std::accumulate( varsDerivatives.begin(), varsDerivatives.end(), 0.0 );
}

void CNetwork::backpropagation2()
{
	vector<ILayer*> layers;
	m_LayersConfiguration->getLayers( layers );

	size_t layers_count = layers.size();

	size_t layer_i = layers_count - 1;

	do
	{
		ILayer * right_layer = layers[layer_i];
		ILayer * left_layer = layers[layer_i - 1];
		size_t right_layer_neurons_count = right_layer->getNeuronsCount();
		size_t left_layer_neurons_count = left_layer->getNeuronsCount();

		//Update bias weights
		for( size_t right_neuron_i = 0 ; right_neuron_i < right_layer_neurons_count ; right_neuron_i++ )
		{
			INeuron * right_layer_neuron = right_layer->getNeuron( right_neuron_i );
			double errorDerivative = 0;
			if( layer_i == layers_count - 1 )
			{
				errorDerivative = ( right_layer_neuron->getError() );//dDk/dyi = d((y1 - a1)^2 + (y2 - a2)^2 + ... + (yn - an)^2)
			}
			else
			{
				errorDerivative = right_layer_neuron->getError();
			}


			double right_output = right_layer_neuron->getOutput();
			double outputDerivative = right_layer_neuron->calculateDerivative( right_output );//dF'(Si)

			double weightGradient = errorDerivative * outputDerivative * BIASOUTPUT;

			double oldGradient = right_layer_neuron->getWeights()[0].gradient();

			if( weightGradient * oldGradient < 0 )//different sign
			{
				double oldLearningRate = right_layer_neuron->getWeights()[0].learningRate();
				double newLearningRate = oldLearningRate * 0.95;
				right_layer_neuron->getWeights()[0].learningRate( newLearningRate );
			}
			else//the same sign
			{
				double oldLearningRate = right_layer_neuron->getWeights()[0].learningRate();
				double newLearningRate = oldLearningRate + 0.05;
				right_layer_neuron->getWeights()[0].learningRate( newLearningRate );
			}

			//Bias
			double oldWeight = right_layer_neuron->getWeights()[0].value();
			double newWeight = oldWeight + ( -1 ) * right_layer_neuron->getWeights()[0].learningRate() * weightGradient;// + oldGradient * m_MomentRate;

			right_layer_neuron->getWeights()[0].value( newWeight );

			right_layer_neuron->getWeights()[0].gradient( weightGradient );
		}

		//Update recent neurons
		for( size_t left_neuron_i = 0 ; left_neuron_i < left_layer_neurons_count ; left_neuron_i++ )
		{
			double errorGradient = 0.0;
			for( size_t right_neuron_i = 0 ; right_neuron_i < right_layer_neurons_count ; right_neuron_i++ )
			{
				INeuron * right_layer_neuron = right_layer->getNeuron( right_neuron_i );
				double errorDerivative = 0;
				if( layer_i == ( layers_count - 1 ) )
				{
					errorDerivative = ( right_layer_neuron->getError() );//dDk/dyi = d((y1 - a1)^2 + (y2 - a2)^2 + ... + (yn - an)^2)
				}
				else
				{
					errorDerivative = right_layer_neuron->getError();
				}


				double right_output = right_layer_neuron->getOutput();
				double outputDerivative = right_layer_neuron->calculateDerivative( right_output );//dF'(Si)

				double left_output = left_layer->getNeuron( left_neuron_i )->getOutput();

				double weightGradient = errorDerivative * outputDerivative * left_output;
				double oldGradient = right_layer_neuron->getWeights()[left_neuron_i + 1].gradient();

				if( weightGradient * oldGradient < 0 )//different sign
				{
					double oldLearningRate = right_layer_neuron->getWeights()[left_neuron_i + 1].learningRate();
					double newLearningRate = oldLearningRate * 0.95;
					right_layer_neuron->getWeights()[left_neuron_i + 1].learningRate( newLearningRate );
				}
				else//the same sign
				{
					double oldLearningRate = right_layer_neuron->getWeights()[left_neuron_i + 1].learningRate();
					double newLearningRate = oldLearningRate + .05;
					right_layer_neuron->getWeights()[left_neuron_i + 1].learningRate( newLearningRate );
				}

				double oldWeight = right_layer_neuron->getWeights()[left_neuron_i + 1].value();

				double newWeight = oldWeight + ( -1 ) * right_layer_neuron->getWeights()[left_neuron_i + 1].learningRate() * weightGradient + oldGradient * m_MomentRate;
				right_layer_neuron->getWeights()[left_neuron_i + 1].value( newWeight );

				right_layer_neuron->getWeights()[left_neuron_i + 1].gradient( weightGradient );


				errorGradient += errorDerivative * outputDerivative * oldWeight;//(Dk/dyi)*(dyi/dxj) = 2(y - a)*F'(Si)*Wi
			}

			//propagate error
			left_layer->getNeuron( left_neuron_i )->setError( errorGradient );
		}

		layer_i--;
	}while( layer_i > 0 );
}

void CNetwork::backpropagation2( uint32_t reserved )
{
	vector<CLayerMatrix*> layers;
	m_LayersConfiguration->getLayers( layers );

	int32_t layers_count = layers.size();

	int32_t layer_i = layers_count - 1;

	do
	{
		CLayerMatrix * layer = layers[layer_i];
		size_t right_layer_neurons_count = layer->getOutputNeuronsCount();
		size_t left_layer_neurons_count = layer->getInputNeuronsCount();

		//Update bias weights
		for( uint32_t right_neuron_i = 0 ; right_neuron_i < right_layer_neurons_count ; right_neuron_i++ )
		{
			INeuron * right_layer_neuron = layer->getOutputNeuron( right_neuron_i );
			double errorDerivative = 0;
			if( layer_i == layers_count - 1 )
			{
				errorDerivative = ( right_layer_neuron->getError() );//dDk/dyi = d((y1 - a1)^2 + (y2 - a2)^2 + ... + (yn - an)^2)
			}
			else
			{
				errorDerivative = right_layer_neuron->getError();
			}


			double right_output = right_layer_neuron->getOutput();
			double outputDerivative = right_layer_neuron->calculateDerivative( right_output );//dF'(Si)

			double weightGradient = errorDerivative * outputDerivative * BIASOUTPUT;

			double oldGradient = layer->getGradients().at_element( right_neuron_i, 0 );

			if( weightGradient * oldGradient < 0 )//different sign
			{
				double oldLearningRate = layer->getLearningRate().at_element( right_neuron_i, 0 );
				double newLearningRate = oldLearningRate * 0.95;
				layer->getLearningRate().at_element( right_neuron_i, 0 ) = newLearningRate;
			}
			else//the same sign
			{
				double oldLearningRate = layer->getLearningRate().at_element( right_neuron_i, 0 );
				double newLearningRate = oldLearningRate + 0.05;
				layer->getLearningRate().at_element( right_neuron_i, 0 ) = newLearningRate;
			}

			//Bias
			double oldWeight = layer->getWeights().at_element( right_neuron_i, 0 );
			double learningRate = layer->getLearningRate().at_element( right_neuron_i, 0 );
			double newWeight = oldWeight + ( -1 ) * learningRate * weightGradient;// + oldGradient * m_MomentRate;

			layer->getWeights().at_element( right_neuron_i, 0 ) = newWeight;

			layer->getGradients().at_element( right_neuron_i, 0 ) = weightGradient;

			double oldw_grad = layer->getGradients().at_element( right_neuron_i, 0 );
			int a = 0;
			a++;
		}

		//Update recent neurons
		for( size_t left_neuron_i = 0 ; left_neuron_i < left_layer_neurons_count ; left_neuron_i++ )
		{
			double errorGradient = 0.0;
			for( size_t right_neuron_i = 0 ; right_neuron_i < right_layer_neurons_count ; right_neuron_i++ )
			{
				INeuron * right_layer_neuron = layer->getOutputNeuron( right_neuron_i );
				double errorDerivative = 0;
				if( layer_i == ( layers_count - 1 ) )
				{
					errorDerivative = ( right_layer_neuron->getError() );//dDk/dyi = d((y1 - a1)^2 + (y2 - a2)^2 + ... + (yn - an)^2)
				}
				else
				{
					errorDerivative = right_layer_neuron->getError();
				}


				double right_output = right_layer_neuron->getOutput();
				double outputDerivative = right_layer_neuron->calculateDerivative( right_output );//dF'(Si)

				double left_output = layer->getInputNeuron( left_neuron_i )->getOutput();

				double weightGradient = errorDerivative * outputDerivative * left_output;
				double oldGradient = layer->getGradients().at_element( right_neuron_i, left_neuron_i + 1 );

				if( weightGradient * oldGradient < 0 )//different sign
				{
					double oldLearningRate = layer->getLearningRate().at_element( right_neuron_i, left_neuron_i + 1 );
					double newLearningRate = oldLearningRate * 0.95;
					layer->getLearningRate().at_element( right_neuron_i, left_neuron_i + 1 ) = newLearningRate;
				}
				else//the same sign
				{
					double oldLearningRate = layer->getLearningRate().at_element( right_neuron_i, left_neuron_i + 1 );
					double newLearningRate = oldLearningRate + 0.05;
					layer->getLearningRate().at_element( right_neuron_i, left_neuron_i + 1 ) = newLearningRate;
				}

				double learningRate = layer->getLearningRate().at_element( right_neuron_i, left_neuron_i + 1 );
				double oldWeight = layer->getWeights().at_element( right_neuron_i, left_neuron_i + 1 );

				double newWeight = oldWeight + ( -1 ) * learningRate * weightGradient + oldGradient * m_MomentRate;
				layer->getWeights().at_element( right_neuron_i, left_neuron_i + 1 ) = newWeight;

				layer->getGradients().at_element( right_neuron_i, left_neuron_i + 1 ) = weightGradient;


				errorGradient += errorDerivative * outputDerivative * oldWeight;//(Dk/dyi)*(dyi/dxj) = 2(y - a)*F'(Si)*Wi
			}

			//propagate error
			layer->getInputNeuron( left_neuron_i )->setError( errorGradient );
		}

		layer_i--;
	}while( layer_i > 0 );
}

void CNetwork::backpropagation2( uint32_t reserved, uint32_t reserved2 )
{
	vector<CLayerMatrix*> layers;
	m_LayersConfiguration->getLayers( layers );

	int32_t layers_count = layers.size();

	int32_t layer_i = layers_count - 1;

	do
	{
		CLayerMatrix * layer = layers[layer_i];
		size_t right_layer_neurons_count = layer->getOutputNeuronsCount();
		size_t left_layer_neurons_count = layer->getInputNeuronsCount();

		//Update bias weights
		for( uint32_t right_neuron_i = 0 ; right_neuron_i < right_layer_neurons_count ; right_neuron_i++ )
		{
			INeuron * right_layer_neuron = layer->getOutputNeuron( right_neuron_i );
			double errorDerivative = 0;
			if( layer_i == layers_count - 1 )
			{
				errorDerivative = ( right_layer_neuron->getError() );//dDk/dyi = d((y1 - a1)^2 + (y2 - a2)^2 + ... + (yn - an)^2)
			}
			else
			{
				errorDerivative = right_layer_neuron->getError();
			}


			double right_output = right_layer_neuron->getOutput();
			double outputDerivative = right_layer_neuron->calculateDerivative( right_output );//dF'(Si)

			double weightGradient = errorDerivative * outputDerivative * BIASOUTPUT;

			double oldGradient = layer->getGradients().at_element( right_neuron_i, 0 );

			if( weightGradient * oldGradient < 0 )//different sign
			{
				double oldLearningRate = layer->getLearningRate().at_element( right_neuron_i, 0 );
				double newLearningRate = oldLearningRate * 0.95;
				layer->getLearningRate().at_element( right_neuron_i, 0 ) = newLearningRate;
			}
			else//the same sign
			{
				double oldLearningRate = layer->getLearningRate().at_element( right_neuron_i, 0 );
				double newLearningRate = oldLearningRate + 0.05;
				layer->getLearningRate().at_element( right_neuron_i, 0 ) = newLearningRate;
			}

			//Bias
			double oldWeight = layer->getWeights().at_element( right_neuron_i, 0 );
			double learningRate = layer->getLearningRate().at_element( right_neuron_i, 0 );
			double newWeight = oldWeight + ( -1 ) * learningRate * weightGradient;// + oldGradient * m_MomentRate;

//			layer->getWeights().at_element( right_neuron_i, 0 ) = newWeight;

			layer->getGradients().at_element( right_neuron_i, 0 ) = weightGradient;
		}

		//Update recent neurons
		for( size_t left_neuron_i = 0 ; left_neuron_i < left_layer_neurons_count ; left_neuron_i++ )
		{
			double errorGradient = 0.0;
			for( size_t right_neuron_i = 0 ; right_neuron_i < right_layer_neurons_count ; right_neuron_i++ )
			{
				INeuron * right_layer_neuron = layer->getOutputNeuron( right_neuron_i );
				double errorDerivative = 0;
				if( layer_i == ( layers_count - 1 ) )
				{
					errorDerivative = ( right_layer_neuron->getError() );//dDk/dyi = d((y1 - a1)^2 + (y2 - a2)^2 + ... + (yn - an)^2)
				}
				else
				{
					errorDerivative = right_layer_neuron->getError();
				}


				double right_output = right_layer_neuron->getOutput();
				double outputDerivative = right_layer_neuron->calculateDerivative( right_output );//dF'(Si)

				double left_output = layer->getInputNeuron( left_neuron_i )->getOutput();

				double weightGradient = errorDerivative * outputDerivative * left_output;
				double oldGradient = layer->getGradients().at_element( right_neuron_i, left_neuron_i + 1 );

				if( weightGradient * oldGradient < 0 )//different sign
				{
					double oldLearningRate = layer->getLearningRate().at_element( right_neuron_i, left_neuron_i + 1 );
					double newLearningRate = oldLearningRate * 0.95;
					layer->getLearningRate().at_element( right_neuron_i, left_neuron_i + 1 ) = newLearningRate;
				}
				else//the same sign
				{
					double oldLearningRate = layer->getLearningRate().at_element( right_neuron_i, left_neuron_i + 1 );
					double newLearningRate = oldLearningRate + 0.05;
					layer->getLearningRate().at_element( right_neuron_i, left_neuron_i + 1 ) = newLearningRate;
				}

				double learningRate = layer->getLearningRate().at_element( right_neuron_i, left_neuron_i + 1 );
				double oldWeight = layer->getWeights().at_element( right_neuron_i, left_neuron_i );

				double newWeight = oldWeight + ( -1 ) * learningRate * weightGradient + oldGradient * m_MomentRate;
//				layer->getWeights().at_element( right_neuron_i, left_neuron_i ) = newWeight;

				layer->getGradients().at_element( right_neuron_i, left_neuron_i ) = weightGradient;


				errorGradient += errorDerivative * outputDerivative * oldWeight;//(Dk/dyi)*(dyi/dxj) = 2(y - a)*F'(Si)*Wi
			}

			//propagate error
			layer->getInputNeuron( left_neuron_i )->setError( errorGradient );
		}

		layer_i--;
	}while( layer_i > 0 );
}

//*************************calculate error average*************//
static double errorSquareSum = 0.0;
void sum ( double & error )
{
	errorSquareSum += error;
}

double getRelativeError( vector<double> errors, unsigned int valueable_size )
{
	std::for_each( errors.begin(), errors.begin() + valueable_size, sum );
	double errorSquareSumMean = errorSquareSum/valueable_size;
	return errorSquareSumMean;
}

double CNetwork::Learn( vector<TrainingData> & trainData, double error_threshold, unsigned int max_epoch )
{
	size_t train_data_count = trainData.size();
	double relativeError = 1000.0;
	unsigned int epoch_i = 0;
	EpochState epochState = { *m_LayersConfiguration, this, 0.0 };

	for(  ; ( relativeError > error_threshold ) && epoch_i < max_epoch ; )
	{
		double meanSquareErrorSum = 0;
		for( size_t train_data_i = 0 ; train_data_i < train_data_count ; train_data_i++ )
		{
			vector<double> & inputs = trainData[train_data_i].input;
			vector<double> & output = trainData[train_data_i].output;

			{
				meanSquareErrorSum += forward( inputs, output, 0 );
				backpropagation2( 0 );

//				printInputLayer(*this);
			}
		}

		relativeError = meanSquareErrorSum / train_data_count;//getRelativeError( errors_per_epoch, epoch_i );

		if( m_epochStateCallback )
		{
			epochState.squareErrorSum = relativeError;
			epochState.index = epoch_i;
			m_epochStateCallback( epochState );
		}

		epoch_i++;
	}

	printf("epoch_i=%d\n", epoch_i);fflush(stdout);
	printf("relativeError=%.10f\n", relativeError);fflush(stdout);

	return relativeError;
}

lbfgsfloatval_t CNetwork::evaluate( void *instance, const lbfgsfloatval_t *x_in, lbfgsfloatval_t *gradients_out, const int n, const lbfgsfloatval_t step )
{
	static int counter = 0;
	counter++;
//	printf("evaluate counter=%d:\n", counter);fflush(stdout);

	LBFGSEvaluateData *data = (LBFGSEvaluateData *)instance;

	vector<CLayerMatrix*> layers;
	data->lpThis->m_LayersConfiguration->getLayers( layers );

	uint32_t layersCount = layers.size();

	for( uint32_t layer_i = 0 ; layer_i < layersCount ; layer_i++ )
	{
		CLayerMatrix * layer = layers[layer_i];
		std::copy( x_in, x_in + n, layer->getWeights().data().begin() );
	}

//	for_each( x_in, x_in + n, [&]( lbfgsfloatval_t value ) { cout << " " << value ; } );
//
//	cout << endl;

//	printf("%f==%f\n", layers[0]->getWeights().data()[layers[0]->getWeights().data().size() - 1], x_in[n - 1] );fflush(stdout);

    double meanSquareErrorSum = data->lpThis->forward( data->trainData->input, data->trainData->output, 0 );
    data->lpThis->backpropagation2( 0, 0 );

	for( size_t layer_i = 0 ; layer_i < layersCount ; layer_i++ )
	{
		CLayerMatrix * layer = layers[layer_i];

		uint32_t rowsCount = layer->getWeights().size1();
		uint32_t colsCount = layer->getWeights().size2();

		std::copy( layer->getGradients().data().begin(), layer->getGradients().data().end(), gradients_out );
	}

    return meanSquareErrorSum;
}

static int progress(
    void *instance,
    const lbfgsfloatval_t *x,
    const lbfgsfloatval_t *g,
    const lbfgsfloatval_t fx,
    const lbfgsfloatval_t xnorm,
    const lbfgsfloatval_t gnorm,
    const lbfgsfloatval_t step,
    int n,
    int k,
    int ls
    )
{
    printf("Iteration %d:\n", k);
    printf("  fx = %f, x[0] = %f, x[1] = %f, x[2] = %f\n", fx, x[0], x[1], x[2]);
    printf("  xnorm = %f, gnorm = %f, step = %f\n", xnorm, gnorm, step);
    printf("\n");
    return 0;
}

double CNetwork::LearnLBFGS( vector<TrainingData> & trainData, double error_threshold, unsigned int max_epoch )
{
	size_t train_data_count = trainData.size();
	double relativeError = 1000.0;
	unsigned int epoch_i = 0;
	EpochState epochState = { *m_LayersConfiguration, this, 0.0 };

	for(  ; /*( relativeError > error_threshold ) && */ epoch_i < max_epoch ; )
	{
		double meanSquareErrorSum = 0;
		for( size_t train_data_i = 0 ; train_data_i < train_data_count ; train_data_i++ )
		{
			lbfgsfloatval_t fx;
			LBFGSEvaluateData data;
			data.lpThis = this;
			data.trainData = &trainData[train_data_i];

			vector<CLayerMatrix*> layers;
			m_LayersConfiguration->getLayers( layers );

			uint32_t layersCount = layers.size();

			for( uint32_t layer_i = 0 ; layer_i < layersCount ; layer_i++ )
			{
				CLayerMatrix * layer = layers[layer_i];
				uint32_t weightsCount = layer->getWeights().data().size();
				std::copy( layer->getWeights().data().begin(), layer->getWeights().data().end(), m_X );
			}

			int32_t ret = lbfgs( m_weightsCount, m_X, &fx, evaluate, progress, &data, &m_LbfgsParam );

			printf("L-BFGS optimization terminated with status code = %d\n", ret);fflush(stdout);

			for( uint32_t layer_i = 0 ; layer_i < layersCount ; layer_i++ )
			{
				CLayerMatrix * layer = layers[layer_i];
				std::copy( m_X, m_X + m_weightsCount, layer->getWeights().data().begin() );
			}

			vector<double> & inputs = trainData[train_data_i].input;
			vector<double> & output = trainData[train_data_i].output;

			{
				meanSquareErrorSum += forward( inputs, output, 0 );
			}
		}

		if( m_epochStateCallback )
		{
			epochState.squareErrorSum = meanSquareErrorSum;
			epochState.index = epoch_i;
			m_epochStateCallback( epochState );
		}

		epoch_i++;

		relativeError = meanSquareErrorSum / train_data_count;//getRelativeError( errors_per_epoch, epoch_i );

	}

	printf("epoch_i=%d\n", epoch_i);fflush(stdout);
	printf("relativeError=%.10f\n", relativeError);fflush(stdout);

	return relativeError;
}

double CNetwork::Test( vector<double> & inputs, vector<double> & output_result )
{
	vector<ILayer*> layers;
	m_LayersConfiguration->getLayers( layers );

	size_t layers_count = layers.size();

	ILayer * last_layer = layers[layers_count - 1];

	double squareErrorSum = 0;

	vector<double> output( layers[layers.size() - 1]->getNeuronsCount(), 0.0 );
	squareErrorSum = forward( inputs, output, 0 );


	size_t last_layer_neurons_count = last_layer->getNeuronsCount();

	output_result.resize( last_layer_neurons_count );

	for( size_t neuron_i = 0 ; neuron_i < last_layer_neurons_count ; neuron_i++ )
	{
		output_result[neuron_i] = last_layer->getNeuron( neuron_i )->getOutput();
	}

	return squareErrorSum;
}

double CNetwork::Test( vector<double> & inputs, vector<double> & output_result, uint32_t reserved )
{
	vector<CLayerMatrix*> layers;
	m_LayersConfiguration->getLayers( layers );

	size_t layers_count = layers.size();

	CLayerMatrix * last_layer = layers[layers_count - 1];

	double squareErrorSum = 0;

	vector<double> output( layers[layers.size() - 1]->getOutputNeuronsCount(), 0.0 );
	squareErrorSum = forward( inputs, output, 0 );


	size_t last_layer_neurons_count = last_layer->getOutputNeuronsCount();

	output_result.resize( last_layer_neurons_count );

	for( size_t neuron_i = 0 ; neuron_i < last_layer_neurons_count ; neuron_i++ )
	{
		output_result[neuron_i] = last_layer->getOutputNeuron( neuron_i )->getOutput();
	}

	return squareErrorSum;
}

bool CNetwork::save( string & filename )
{
//	vector<CLayerMatrix*> layers;
//	m_LayersConfiguration->getLayers( layers );
//
//	size_t layers_count = layers.size();
//
//	ofstream file( filename.c_str() );
//
//	if( false == file.is_open() )
//	{
//		return false;
//	}
//
//	file << layers_count << endl << flush;
//
//	for( size_t layer_i = 0 ; layer_i < layers_count ; layer_i++ )
//	{
//		CLayerMatrix * layer = layers[layer_i];
//		size_t neurons_count = layer->getInputNeuronsCount();
//		file << neurons_count << flush;
//		if( layer_i < ( layers_count - 1 ) )
//		{
//			file << " " << flush;
//		}
//
//		neurons_count = layer->getOutputNeuronsCount();
//		file << neurons_count << flush;
//		if( layer_i < ( layers_count - 1 ) )
//		{
//			file << " " << flush;
//		}
//	}
//
//	file << endl << flush;
//
//	for( size_t layer_i = 1 ; layer_i < layers_count ; layer_i++ )
//	{
//		CLayerMatrix * layer = layers[layer_i];
//		size_t neurons_count = layer->getNeuronsCount();
//		for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
//		{
//			INeuron * neuron = layer->getNeuron( neuron_i );
//			vector<Weight> & weights = neuron->getWeights();
//			size_t weights_count = weights.size();
//			file << weights_count << endl << flush;
//			for( size_t weight_i = 0 ; weight_i < weights_count ; weight_i++ )
//			{
//				file << weights[weight_i].value() << flush;
//				if( weight_i < ( weights_count - 1 ) )
//				{
//					file << " " << flush;
//				}
//			}
//			file << endl << flush;
//		}
//	}
//
//	file.close();

	return true;
}

bool CNetwork::load( string & filename )
{
	ifstream file( filename.c_str() );
	if( false == file.is_open() )
	{
		return false;
	}

	size_t layers_count = 0;

	file >> layers_count;

	vector<size_t> layers_sizes;

	for( size_t layer_i = 0 ; layer_i < layers_count ; layer_i++ )
	{
		size_t neurons_count = 0;
		file >> neurons_count;
		layers_sizes.push_back( neurons_count );
	}

	vector<size_t> hidden_layers_sizes;
	if( layers_count > 2 )
	{
		hidden_layers_sizes.assign( layers_sizes.begin() + 1, layers_sizes.begin() + layers_count - 1 );
	}
	CLayersConfiguration * layersConfuration = new CLayersConfiguration( layers_sizes[0], layers_sizes[layers_count - 1], hidden_layers_sizes );

	vector<ILayer*> layers;
	layersConfuration->getLayers( layers );

	layers_count = layers.size();

	for( size_t layer_i = 1 ; layer_i < layers_count ; layer_i++ )
	{
		size_t neurons_count = layers_sizes[layer_i];

		for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
		{
			vector<Weight> & weights = layers[layer_i]->getNeuron( neuron_i )->getWeights();
			weights.clear();
			size_t weights_count = 0;
			file >> weights_count;
			for( size_t weight_i = 0 ; weight_i < weights_count; weight_i++ )
			{
				float weight = 0;
				file >> weight;
				weights.push_back( weight );

				if( file.eof() == true )
				{
					break;
				}
			}
		}
	}

	m_LayersConfiguration = layersConfuration;

	return true;
}

void CNetwork::setEpochStateCallback( EpochStateCallback epochStateCallback )
{
	m_epochStateCallback = epochStateCallback;
}

void CNetwork::reverse()
{
//	CLayersConfiguration * prevLayersConfuration = m_LayersConfiguration;
//	vector<ILayer*> layers;
//	prevLayersConfuration->getLayers( layers );
//
//	size_t layers_count = layers.size();
//
//
//	vector<size_t> layers_sizes;
//
//	for( int layer_i = layers_count - 1 ; layer_i >= 0 ; layer_i-- )
//	{
//		size_t neurons_count = layers[layer_i]->getNeuronsCount();
//		layers_sizes.push_back( neurons_count );
//	}
//
//	vector<size_t> hidden_layers_sizes;
//	if( layers_count > 2 )
//	{
//		std::reverse( layers_sizes.begin(), layers_sizes.end() );
//		hidden_layers_sizes.assign( layers_sizes.begin() + 1, layers_sizes.begin() + layers_count - 1 );
//		std::reverse( layers_sizes.begin(), layers_sizes.end() );
//		int s = hidden_layers_sizes[0];
//	}
//
//	CLayersConfiguration * newLayersConfuration = new CLayersConfiguration( layers_sizes[0], layers_sizes[layers_count - 1], hidden_layers_sizes );
//
//	vector<ILayer*> newLayers;
//	vector<ILayer*> prevLayers;
//	newLayersConfuration->getLayers( newLayers );
//	prevLayersConfuration->getLayers( prevLayers );
//
//	layers_count = layers.size();
//
//	for( size_t new_layer_i = 1 ; new_layer_i < layers_count ; new_layer_i++ )
//	{
//		ILayer *newLayer = newLayers[new_layer_i];
//		ILayer *prevLayer = prevLayers[layers_count - new_layer_i];
//
//		size_t new_layer_neurons_count = newLayer->getNeuronsCount();
//
//		for( size_t new_layer_neuron_i = 0 ; new_layer_neuron_i < new_layer_neurons_count ; new_layer_neuron_i++ )
//		{
//			size_t prev_layer_neurons_count = prevLayer->getNeuronsCount();
//
//			vector<double> newWeights( prev_layer_neurons_count /*bias*/, 0.0 );
//
//			for( size_t prev_layer_neuron_i = 0 ; prev_layer_neuron_i < prev_layer_neurons_count ; prev_layer_neuron_i++ )
//			{
//				vector<double> prevWeights;
//				prevLayer->getNeuron( prev_layer_neuron_i )->getWeights( prevWeights );
//
//				size_t prev_layer_weights_count = prevWeights.size();
//
//				double new_layer_neuron_weight = prevWeights[new_layer_neuron_i + 1/*bias*/];
//				newWeights[prev_layer_neuron_i/*bias*/] = new_layer_neuron_weight;
//
//				/**Bias*/
//
//
//			}
//
////			double bias = prevWeights[0/*bias*/];;
////			newWeights[0] = bias;
//
//			newLayer->getNeuron(new_layer_neuron_i)->setWeights( newWeights );
//		}
//	}
//
//	m_LayersConfiguration = newLayersConfuration;
}


















