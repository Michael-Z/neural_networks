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

#define BIASOUTPUT 1   //output for bias. It's always 1.

CNetwork::CNetwork( CLayersConfiguration & sequence, double learningRateStart, double momentRate ): m_fromFile( false ), m_LearningRate( learningRateStart ), m_MomentRate( momentRate ), m_LayersConfiguration( &sequence ), m_epochStateCallback( NULL )
{
	createNetwork();
}

CNetwork::CNetwork( string & filename ): m_fromFile( true ), m_LearningRate( 0 ), m_MomentRate( 0 ), m_LayersConfiguration( NULL ), m_epochStateCallback( NULL )
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
		}
		currlayer->initWeightsParams( m_LearningRate );
	}


}

//void printInputLayer( CNetwork & network )
//{
//	vector<ILayer*> layers;
//	network.m_LayersConfiguration->getLayers( layers );
//
//	size_t neurons_count = layers[0]->getNeuronsCount();
//
//	for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
//	{
//		printf( "input[%d]=%f\n", (int)neuron_i, layers[0]->getNeuron( neuron_i )->getOutput() ); fflush( stdout );
//	}
//}

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

//	for( size_t neuron_i = 0 ; neuron_i < last_layer->getNeuronsCount() ; neuron_i++ )
//	{
//		double error = last_layer->( outputData, neuron_i );// last_layer->getNeuron( neuron_i )->getOutput() - outputData[neuron_i];//y - a
//		last_layer->getNeuron( neuron_i )->setError( error );
//
//		sum_square_error += ( error*error );
//	}

	double mean_square_error = sum_square_error / last_layer->getNeuronsCount();

	return mean_square_error;

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
				errorDerivative = ( 2 * right_layer_neuron->getError() );//dDk/dyi = d((y1 - a1)^2 + (y2 - a2)^2 + ... + (yn - an)^2)
			}
			else
			{
				errorDerivative = right_layer_neuron->getError();
			}


			double right_output = right_layer_neuron->getOutput();
			double outputDerivative = right_layer_neuron->calculateDerivative( right_output );//dF'(Si)

			double weightGradient = errorDerivative * outputDerivative * BIASOUTPUT;

			double oldGradient = right_layer_neuron->getWeights()[0].gradient();

			if( ( weightGradient + oldGradient ) < weightGradient )//different sign
			{
				double oldLearningRate = right_layer_neuron->getWeights()[0].learningRate();
				double newLearningRate = oldLearningRate / 10.0;
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
					errorDerivative = ( 2 * right_layer_neuron->getError() );//dDk/dyi = d((y1 - a1)^2 + (y2 - a2)^2 + ... + (yn - an)^2)
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
					double newLearningRate = oldLearningRate * 0.999995;
					right_layer_neuron->getWeights()[left_neuron_i + 1].learningRate( newLearningRate );
				}
				else//the same sign
				{
					double oldLearningRate = right_layer_neuron->getWeights()[left_neuron_i + 1].learningRate();
					double newLearningRate = oldLearningRate + 0.000005;
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

//*************************calculate error average*************//

double getRelativeError( vector<double> errors, unsigned int valueable_size )
{
	double errorSquareSum = 0.0;
	std::for_each( errors.begin(), errors.begin() + valueable_size,
																	 [ & ] ( double & error )
																	 {
																		errorSquareSum += error;
																	 });
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
				meanSquareErrorSum += forward( inputs, output );
				backpropagation2();
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
	squareErrorSum = forward( inputs, output );


	size_t last_layer_neurons_count = last_layer->getNeuronsCount();

	output_result.resize( last_layer_neurons_count );

	for( size_t neuron_i = 0 ; neuron_i < last_layer_neurons_count ; neuron_i++ )
	{
		output_result[neuron_i] = last_layer->getNeuron( neuron_i )->getOutput();
	}

	return squareErrorSum;
}

bool CNetwork::save( string & filename )
{
	vector<ILayer*> layers;
	m_LayersConfiguration->getLayers( layers );

	size_t layers_count = layers.size();

	ofstream file( filename.c_str() );

	if( false == file.is_open() )
	{
		return false;
	}

	file << layers_count << endl << flush;

	for( size_t layer_i = 0 ; layer_i < layers_count ; layer_i++ )
	{
		ILayer * layer = layers[layer_i];
		size_t neurons_count = layer->getNeuronsCount();
		file << neurons_count << flush;
		if( layer_i < ( layers_count - 1 ) )
		{
			file << " " << flush;
		}
	}

	file << endl << flush;

	for( size_t layer_i = 1 ; layer_i < layers_count ; layer_i++ )
	{
		ILayer * layer = layers[layer_i];
		size_t neurons_count = layer->getNeuronsCount();
		for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
		{
			INeuron * neuron = layer->getNeuron( neuron_i );
			vector<Weight> & weights = neuron->getWeights();
			size_t weights_count = weights.size();
			file << weights_count << endl << flush;
			for( size_t weight_i = 0 ; weight_i < weights_count ; weight_i++ )
			{
				file << weights[weight_i].value() << flush;
				if( weight_i < ( weights_count - 1 ) )
				{
					file << " " << flush;
				}
			}
			file << endl << flush;
		}
	}

	file.close();

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


















