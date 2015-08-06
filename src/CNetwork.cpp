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
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>

CNetwork::CNetwork( CLayersConfiguration & sequence ): m_fromFile( false ), m_LearningRate( ETA ), m_LayersConfiguration( &sequence ), m_epochStateCallback( NULL )
{
	createNetwork();
}

CNetwork::CNetwork( string & filename ): m_fromFile( true ), m_LearningRate( ETA ), m_LayersConfiguration( NULL ), m_epochStateCallback( NULL )
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
			vector<double> weights( prevlayer->getNeuronsCount() + 1, 0.0 );
			random.fillVector( weights );

			currlayer->setWeights( neuron_i, weights );
		}
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
			double multipliedSumWeight = 0.0;
			vector<double> & weights = layer->getNeuron( neuron_i )->getWeights();
			for( size_t prev_layer_neuron_i = 0 ; prev_layer_neuron_i <= prev_layer_neurons_count ; prev_layer_neuron_i++ )
			{
				double multiplied_weight = 0.0;

				if( prev_layer_neuron_i == 0 )
				{
					multiplied_weight = weights[prev_layer_neuron_i] * BIASOUTPUT;
				}
				else
				{
					double output = prevLayer->getNeuron( prev_layer_neuron_i - 1 )->getOutput();
					multiplied_weight = weights[prev_layer_neuron_i] * output;
				}

				multipliedSumWeight += multiplied_weight;
			}

			layer->getNeuron( neuron_i )->setOutput( multipliedSumWeight );
		}
	}

	double errorsSquaresSum = 0.0;

	ILayer * last_layer = layers[layers_count - 1];

	for( size_t neuron_i = 0 ; neuron_i < last_layer->getNeuronsCount() ; neuron_i++ )
	{
		double error = outputData[neuron_i] - last_layer->getNeuron( neuron_i )->getOutput();
		double errorsTwice = 2 * error;//Derivative of Dk by yi
		last_layer->getNeuron( neuron_i )->setError( errorsTwice );

		double errorsSquare = error * error;
		errorsSquaresSum += errorsSquare;//Dk = (y1 - a1)^2 + (y2 - a2)^2 + ... + (yn - an)^2
	}

	return errorsSquaresSum;
}
/*
void CNetwork::backpropagation()
{
	vector<ILayer*> layers;
	m_LayersConfiguration->getLayers( layers );

	size_t layers_count = layers.size();

	ILayer * last_layer = layers[layers_count - 1];

	for( size_t neuron_i = 0 ; neuron_i < last_layer->getNeuronsCount() ; neuron_i++ )
	{
		INeuron * neuron = last_layer->getNeuron( neuron_i );
		double output = neuron->getOutput();
		double error = neuron->getError();

		double delta = output * ( 1 - output ) * error;
		neuron->setDelta( delta );
	}

	vector<double> next_layer_weights;

	//calculate delta for hidden layers
	for( size_t layer_i = layers_count - 2 ; layer_i > 0 ; layer_i-- ) // move only in hidden layers
	{
		ILayer * currLayer = layers[layer_i];
		ILayer * nextLayer = layers[layer_i + 1];
		size_t neurons_count = currLayer->getNeuronsCount();
		size_t next_layer_neurons_count = nextLayer->getNeuronsCount();
		for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
		{

			double output = currLayer->getNeuron( neuron_i )->getOutput();
			double sum = 0.0;
			for( size_t next_layer_neuron_i = 0 ; next_layer_neuron_i < next_layer_neurons_count ; next_layer_neuron_i++ )
			{
				nextLayer->getNeuron( next_layer_neuron_i )->getWeights( next_layer_weights );
				double weight = next_layer_weights[neuron_i + 1];
				sum += weight * nextLayer->getNeuron( next_layer_neuron_i )->getDelta();
			}
			double delta = output * ( 1 - output ) * sum;
			currLayer->getNeuron( neuron_i )->setDelta( delta );
		}
	}

	//update weights
	for( size_t layer_i = 1 ; layer_i < layers_count ; layer_i++ )
	{
		ILayer * layer = layers[layer_i];
		ILayer * prevLayer = layers[layer_i - 1];
		size_t neurons_count = layer->getNeuronsCount();
		for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
		{
			INeuron * currNeuron = layer->getNeuron( neuron_i );
			vector<double> weights;
			currNeuron->getWeights( weights );
			size_t weights_count = weights.size();
			for( size_t weight_i = 0 ; weight_i < weights_count ; weight_i++ )
			{
				double inputForThisNeuron = 0.0;
				if( weight_i == 0 )
				{
					inputForThisNeuron = 1.0;
				}
				else
				{
					size_t prev_layer_neuron_assoc_with_weight_i = weight_i - 1;
					inputForThisNeuron = prevLayer->getNeuron( prev_layer_neuron_assoc_with_weight_i )->getOutput();
				}

				weights[weight_i] += m_LearningRate * currNeuron->getDelta() * inputForThisNeuron;
			}
			currNeuron->setWeights( weights );
		}
	}
}
*/

void CNetwork::backpropagation()
{
	vector<ILayer*> layers;
	m_LayersConfiguration->getLayers( layers );

	size_t layers_count = layers.size();

	size_t layer_i = layers_count - 1 ;
	do
	{

		ILayer * right_layer = layers[layer_i];
		ILayer * left_layer = layers[layer_i - 1];
		size_t right_layer_neurons_count = right_layer ->getNeuronsCount();
		size_t left_layer_neurons_count = left_layer ->getNeuronsCount();

		for( size_t neuron_i = 0 ; neuron_i < right_layer_neurons_count ; neuron_i++ )
		{
			double weightGradient = 0;
			double outputDerivative = 0;
			double errorDerivative = 0;
			double output = right_layer ->getNeuron( neuron_i )->getOutput();
			if( layer_i == layers_count - 1 )
			{
				errorDerivative = ( 2 * right_layer ->getNeuron( neuron_i )->getError() );
			}
			else
			{
				errorDerivative = right_layer ->getNeuron( neuron_i )->getError();
			}

			outputDerivative = output * ( 1 - output );

			double biasWeightGradient = ( -1 ) * m_LearningRate * errorDerivative * outputDerivative * 1;

			vector<double> weightGradients( prev_layer_neurons_count + 1, 0.0 );
			weightGradients[0] = biasWeightGradient;

			for( size_t neuron_j = 0 ; neuron_j < prev_layer_neurons_count ; neuron_j++ )
			{
				double prev_output = left_layer ->getNeuron( neuron_j )->getOutput();
				weightGradient = ( -1 ) * m_LearningRate * errorDerivative * outputDerivative * prev_output;
				weightGradients[ neuron_j + 1 ] = weightGradient;
			}

			vector<double> & oldWeights = left_layer ->getNeuron( neuron_i )->getWeights();
			size_t weightsCount = oldWeights.size();

			vector<double> updatedWeigth( weightsCount );

			for( size_t weight_i = 0 ; weight_i < weightsCount ; weight_i++ )
			{
				updatedWeigth[ weight_i ] += weightGradients[ weight_i ];
			}
		}

		layer_i--;
	}while( layer_i > 0 );
/*
	for( size_t neuron_i = 0 ; neuron_i < last_layer->getNeuronsCount() ; neuron_i++ )
	{
		INeuron * neuron = last_layer->getNeuron( neuron_i );
		double output = neuron->getOutput();
		double error = neuron->getError();

		double delta = output * ( 1 - output ) * error;
		neuron->setDelta( delta );
	}

	vector<double> next_layer_weights;

	//calculate delta for hidden layers
	for( size_t layer_i = layers_count - 2 ; layer_i > 0 ; layer_i-- ) // move only in hidden layers
	{
		ILayer * currLayer = layers[layer_i];
		ILayer * nextLayer = layers[layer_i + 1];
		size_t neurons_count = currLayer->getNeuronsCount();
		size_t next_layer_neurons_count = nextLayer->getNeuronsCount();
		for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
		{

			double output = currLayer->getNeuron( neuron_i )->getOutput();
			double sum = 0.0;
			for( size_t next_layer_neuron_i = 0 ; next_layer_neuron_i < next_layer_neurons_count ; next_layer_neuron_i++ )
			{
				nextLayer->getNeuron( next_layer_neuron_i )->getWeights( next_layer_weights );
				double weight = next_layer_weights[neuron_i + 1];
				sum += weight * nextLayer->getNeuron( next_layer_neuron_i )->getDelta();
			}
			double delta = output * ( 1 - output ) * sum;
			currLayer->getNeuron( neuron_i )->setDelta( delta );
		}
	}

	//update weights
	for( size_t layer_i = 1 ; layer_i < layers_count ; layer_i++ )
	{
		ILayer * layer = layers[layer_i];
		ILayer * prevLayer = layers[layer_i - 1];
		size_t neurons_count = layer->getNeuronsCount();
		for( size_t neuron_i = 0 ; neuron_i < neurons_count ; neuron_i++ )
		{
			INeuron * currNeuron = layer->getNeuron( neuron_i );
			vector<double> weights;
			currNeuron->getWeights( weights );
			size_t weights_count = weights.size();
			for( size_t weight_i = 0 ; weight_i < weights_count ; weight_i++ )
			{
				double inputForThisNeuron = 0.0;
				if( weight_i == 0 )
				{
					inputForThisNeuron = 1.0;
				}
				else
				{
					size_t prev_layer_neuron_assoc_with_weight_i = weight_i - 1;
					inputForThisNeuron = prevLayer->getNeuron( prev_layer_neuron_assoc_with_weight_i )->getOutput();
				}

				weights[weight_i] += m_LearningRate * currNeuron->getDelta() * inputForThisNeuron;
			}
			currNeuron->setWeights( weights );
		}
	}
	*/
}

//*************************calculate error average*************//

//double getRelativeError( vector<double> errors, unsigned int valueable_size )
//{
//	size_t errors_count = valueable_size;
//
//	//keep track of the last 20 Root of Mean Square Errors
//	size_t start1 = 0;
//	size_t error_count_middle = errors_count / 2;
//	size_t start2 = 0;
//	if( error_count_middle == 0 )
//	{
//		start1 = 0;
//		start2 = errors_count;
//	}
//	else
//	{
//		start1 = 0;
//		start2 = error_count_middle;
//	}
//
//	double error1 = 0;
//	double error2 = 0;
//
//	//calculate the average of the first 10 errors
//	for( size_t error_i = start1 ; error_i < start2; error_i++)
//	{
//		error1 += errors[error_i];
//	}
//	double averageError1 = error1 / error_count_middle;
//
//	//calculate the average of the second 10 errors
//	for( size_t error_i = start2; error_i < errors_count; error_i++)
//	{
//		error2 += errors[error_i];
//	}
//	double averageError2 = error2 / error_count_middle;
//
//	double relativeErr = (averageError1 - averageError2)/averageError1;
//	return (relativeErr > 0) ? relativeErr : -relativeErr;
//}

double getRelativeError( vector<double>& errors )
{
	double squaresSummma = 0;
	std::for_each(errors.begin(), errors.end(),
											  [&](double& error)
											  {
											  	  squaresSummma += error*error;
											  }
	);

	return squaresSummma;
}

double CNetwork::Learn( vector<TrainingData> & trainData, double error_threshold, unsigned int max_epoch )
{
	size_t train_data_count = trainData.size();
	vector<double> errors_per_epoch( max_epoch, 0.0 );
	double relativeError = 1000.0;
	unsigned int epoch_i = 0;
	EpochState epochState = { *m_LayersConfiguration, 0.0 };
	for(  ; ( relativeError > error_threshold ) && epoch_i < max_epoch ; )
	{
		double errorsSquaresSum = 0;
		for( size_t train_data_i = 0 ; train_data_i < train_data_count ; train_data_i++ )
		{
			vector<double> & inputs = trainData[train_data_i].input;
			vector<double> & output = trainData[train_data_i].output;

			{
				errorsSquaresSum += forward( inputs, output );
				backpropagation();
				epochState.squareErrorSum = errorsSquaresSum;
			}
		}

//		if( epoch_i % 10 )
		{
			if( m_epochStateCallback )
			{
				m_epochStateCallback( epochState );
			}
		}

		errors_per_epoch[epoch_i] = errorsSquaresSum;

		epoch_i++;

//		if( epoch_i % 2 == 0 )
		{
			relativeError = getRelativeError( errors_per_epoch );
		}
	}

	return relativeError;
}

double CNetwork::Test( vector<double> & inputs, vector<double> & output_result )
{
	vector<ILayer*> layers;
	m_LayersConfiguration->getLayers( layers );

	size_t layers_count = layers.size();

	ILayer * last_layer = layers[layers_count - 1];

	size_t inputs_count = inputs.size();
	double squareErrorSum = 0;

	vector<double> output( layers[layers.size() - 1]->getNeuronsCount(), 0.0 );
	squareErrorSum += forward( inputs, output );


	size_t last_layer_neurons_count = last_layer->getNeuronsCount();

	output_result.resize( last_layer_neurons_count );

	for( size_t neuron_i = 0 ; neuron_i < last_layer_neurons_count ; neuron_i++ )
	{
		output_result[neuron_i] = last_layer->getNeuron( neuron_i )->getOutput();
	}

	return sqrt( squareErrorSum / inputs_count );
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
			vector<double> & weights = neuron->getWeights();
			size_t weights_count = weights.size();
			file << weights_count << endl << flush;
			for( size_t weight_i = 0 ; weight_i < weights_count ; weight_i++ )
			{
				file << weights[weight_i] << flush;
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
			vector<double> weights;
			size_t weights_count = 0;
			file >> weights_count;
			for( size_t weight_i = 0 ; weight_i < weights_count && (file.eof() != true); weight_i++ )
			{
				float weight = 0;
				file >> weight;
				weights.push_back( weight );
			}
			weights_count = weights.size();
			layers[layer_i]->getNeuron( neuron_i )->setWeights( weights );
		}
	}

	m_LayersConfiguration = layersConfuration;

	return true;
}

void CNetwork::setEpochStateCallback( EpochStateCallback epochStateCallback )
{
	m_epochStateCallback = epochStateCallback;
}

void CNetwork::setLearnRate( double learnRate )
{
	m_LearningRate = learnRate;
}

void CNetwork::reverse()
{
	CLayersConfiguration * prevLayersConfuration = m_LayersConfiguration;
	vector<ILayer*> layers;
	prevLayersConfuration->getLayers( layers );

	size_t layers_count = layers.size();


	vector<size_t> layers_sizes;

	for( int layer_i = layers_count - 1 ; layer_i >= 0 ; layer_i-- )
	{
		size_t neurons_count = layers[layer_i]->getNeuronsCount();
		layers_sizes.push_back( neurons_count );
	}

	vector<size_t> hidden_layers_sizes;
	if( layers_count > 2 )
	{
		std::reverse( layers_sizes.begin(), layers_sizes.end() );
		hidden_layers_sizes.assign( layers_sizes.begin() + 1, layers_sizes.begin() + layers_count - 1 );
		std::reverse( layers_sizes.begin(), layers_sizes.end() );
		int s = hidden_layers_sizes[0];
	}

	CLayersConfiguration * newLayersConfuration = new CLayersConfiguration( layers_sizes[0], layers_sizes[layers_count - 1], hidden_layers_sizes );

	vector<ILayer*> newLayers;
	vector<ILayer*> prevLayers;
	newLayersConfuration->getLayers( newLayers );
	prevLayersConfuration->getLayers( prevLayers );

	layers_count = layers.size();

	for( size_t new_layer_i = 1 ; new_layer_i < layers_count ; new_layer_i++ )
	{
		ILayer *newLayer = newLayers[new_layer_i];
		ILayer *prevLayer = prevLayers[layers_count - new_layer_i];

		size_t new_layer_neurons_count = newLayer->getNeuronsCount();

		for( size_t new_layer_neuron_i = 0 ; new_layer_neuron_i < new_layer_neurons_count ; new_layer_neuron_i++ )
		{
			size_t prev_layer_neurons_count = prevLayer->getNeuronsCount();

			vector<double> newWeights( prev_layer_neurons_count /*bias*/, 0.0 );

			for( size_t prev_layer_neuron_i = 0 ; prev_layer_neuron_i < prev_layer_neurons_count ; prev_layer_neuron_i++ )
			{
				vector<double> prevWeights = prevLayer->getNeuron( prev_layer_neuron_i )->getWeights();

				size_t prev_layer_weights_count = prevWeights.size();

				double new_layer_neuron_weight = prevWeights[new_layer_neuron_i + 1/*bias*/];
				newWeights[prev_layer_neuron_i/*bias*/] = new_layer_neuron_weight;

				/**Bias*/


			}

//			double bias = prevWeights[0/*bias*/];;
//			newWeights[0] = bias;

			newLayer->getNeuron(new_layer_neuron_i)->setWeights( newWeights );
		}
	}

	m_LayersConfiguration = newLayersConfuration;
}


















