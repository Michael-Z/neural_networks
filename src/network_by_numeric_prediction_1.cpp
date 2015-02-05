/*
 * network_by_and.cpp
 *
 *  Created on: Jan 28, 2015
 *      Author: vostanin
 */

#include "network_by_numeric_prediction_1.h"
#include <cstdio>
#include <climits>
#include <cmath>

static void getTestTrainData( TrainingData & data, int start, int end, int predict_expected );
static void dump( vector<double> & input );
static void normalize_range( TrainingData & data, int mupltyplier );
static void denormalize_range( TrainingData & data, int mupltyplier );

void train_network_by_numeric_prediction_1()
{
	const unsigned int input_count = 10;
	const unsigned int output_count = 1;
	vector<size_t> hiddenLayers;
	hiddenLayers.push_back( ( input_count + output_count  ) / 2 );
	hiddenLayers.push_back( ( hiddenLayers[0] + output_count  ) / 2 );
	CLayersConfiguration sequence( input_count, output_count, hiddenLayers );

	CNetwork network( sequence );

	string network_filename( "network_prediction.net" );

	network.save( network_filename );

	vector<TrainingData> trainData_v;

	const size_t train_data_count = 1000;

	size_t train_set_begin = 5000;

	trainData_v.resize( train_data_count, TrainingData() );

	for( unsigned int train_i = 0 ; train_i < train_data_count ; train_i++ )
	{
		getTestTrainData( trainData_v[train_i], train_i + train_set_begin, train_i + train_set_begin + input_count, train_i + train_set_begin + input_count + 1 );

		normalize_range( trainData_v[train_i], 10000 );
	}

	double error_threshold = 1e-5;

	double relativeErrorTrain = network.Learn( trainData_v, error_threshold, 450 );
	printf( "relativeErrorTrain=%f\n", relativeErrorTrain ); fflush( stdout );
	network.save( network_filename );

	const size_t test_data_count = 50;

	size_t test_set_begin = 5000;

	for( unsigned int test_i = 0 ; test_i < test_data_count ; test_i++ )
	{
		TrainingData test_input;
		getTestTrainData( test_input, test_i + test_set_begin, test_i + test_set_begin + input_count, 0 /*not used*/ );

		normalize_range( test_input, 10000 /*not used*/ );

		vector<double> output_test;
		double relativeErrorTest = network.Test( test_input.input, output_test );
		test_input.output = output_test;
//		dump( test_input.output );
		normalize_range( test_input, 10000 /*not used*/ );
		dump( test_input.output );
		double max_value = 0.0;
		for( size_t output_i = 0 ; output_i < output_test.size() ; output_i++ )
		{
			if( max_value < output_test[output_i] )
			{
				max_value = output_test[output_i];
			}
		}

		printf( "relativeErrorTest=%f, image_index=%d\n", relativeErrorTest, (int)max_value ); fflush( stdout );
	}
}

void test_network_by_numeric_prediction_1()
{
	const unsigned int input_count = 10;

	string filename = "network_prediction.net";
	CNetwork network( filename );

	const size_t test_data_count = 50;

	size_t test_set_begin = 5000;

	for( unsigned int test_i = 0 ; test_i < test_data_count ; test_i++ )
	{
		TrainingData test_input;
		getTestTrainData( test_input, test_i + test_set_begin, test_i + test_set_begin + input_count, 0 /*not used*/ );

		normalize_range( test_input, 10000 /*not used*/ );

		vector<double> output_test;
		double relativeErrorTest = network.Test( test_input.input, output_test );
		test_input.output = output_test;
//		dump( test_input.output );
		normalize_range( test_input, 10000 /*not used*/ );
		dump( test_input.output );
		double max_value = 0.0;
		for( size_t output_i = 0 ; output_i < output_test.size() ; output_i++ )
		{
			if( max_value < output_test[output_i] )
			{
				max_value = output_test[output_i];
			}
		}

		printf( "relativeErrorTest=%f, image_index=%d\n", relativeErrorTest, (int)max_value ); fflush( stdout );
	}
}

#include <fstream>

void getTestTrainData( TrainingData & data, int start, int end, int predict_expected )
{
	int count = abs( end - start );

	data.input.resize( count, 0 );

	for( int number_i = start + 1, input_i = 0 ; number_i < ( end + 1 ) ; number_i++, input_i++ )
	{
		data.input[input_i] = number_i;
	}

	data.output.push_back( predict_expected );
}

static void dump( vector<double> & input )
{
	ofstream file( "dump.txt", ios_base::app );

	for( size_t i = 0 ; i < input.size() ; i++ )
	{
		file << input[i] << " " << std::flush;
	}
	file << endl;
	file.close();
}

static void normalize_range( TrainingData & data, int multyplier )
{
	vector<double> & input = data.input;
	vector<double> & output = data.output;

	size_t input_size = input.size();

	for( size_t input_i = 0 ; input_i < input_size ; input_i++ )
	{
		input[input_i] = 1 / input[input_i];
	}

	size_t output_size = output.size();

	for( size_t output_i = 0 ; output_i < output_size ; output_i++ )
	{
		output[output_i] = 1 / output[output_i];
	}
}
//
//static void denormalize_range( TrainingData & data, int multyplier )
//{
//	vector<double> & input = data.input;
//	vector<double> & output = data.output;
//
//	size_t input_size = input.size();
//
//	for( size_t input_i = 0 ; input_i < input_size ; input_i++ )
//	{
//		input[input_i] *= multyplier;
//	}
//
//	size_t output_size = output.size();
//
//	for( size_t output_i = 0 ; output_i < output_size ; output_i++ )
//	{
//		output[output_i] *= multyplier;
//	}
//}
