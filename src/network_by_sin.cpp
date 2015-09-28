/*
 * network_by_sin.cpp
 *
 *  Created on: Jan 28, 2015
 *      Author: vostanin
 */

#include "network_by_sin.h"
#include "Graph.h"
#include <cstdio>
#include <ctime>
#include <cmath>
#include <iostream>
#include <fstream>
using namespace std;

static void getTestTrainData( TrainingData & data, int index, int size );
static void dump( vector<double> & input );
static void epoch_cb(EpochState & epochState);

class Timer
{
public:
  Timer(const std::string& name)
    : name_ (name),
      start_ (std::clock())
    {
    }
  ~Timer()
    {
	  clock_t now = std::clock();
      double elapsed = (double(now - start_) / CLOCKS_PER_SEC);
      std::cout << "start=" << start_ << "; now=" << now << "; " << name_ << ": " << (elapsed * 1000) << " ms" << std::endl;
    }
private:
  std::string name_;
  std::clock_t start_;
};

#define TIMER(name) Timer timer__(name);

void train_network_by_sin()
{
	const unsigned int input_count = 1;
	const unsigned int output_count = 1;
	vector<size_t> hiddenLayers;
	hiddenLayers.push_back( 30 );
	CLayersConfiguration sequence( input_count, output_count, hiddenLayers );

	CNetwork network( sequence, 1, 0.1 );

	string network_filename( "network_sin.net" );

	vector<TrainingData> trainData_v;

	const size_t traine_data_count = 20;

	trainData_v.resize( traine_data_count, TrainingData() );

	for( unsigned int traine_i = 0 ; traine_i < traine_data_count ; traine_i++ )
	{
		getTestTrainData( trainData_v[traine_i], traine_i, traine_data_count );
	}

	double error_threshold = 1e-7;

	network.setEpochStateCallback( epoch_cb );

	double relativeErrorTrain = 0.0;
	{
		TIMER("train")

		relativeErrorTrain = network.Learn( trainData_v, error_threshold, 1000000 );
	}
	printf( "relativeErrorTrain=%f\n", relativeErrorTrain ); fflush( stdout );
	network.save( network_filename );
}

void test_network_by_sin()
{
	const unsigned int input_count = 2;
	const unsigned int output_count = 1;
	vector<size_t> hiddenLayers;
	CLayersConfiguration sequence( input_count, output_count, hiddenLayers );

	string filename = "network_sin.net";
	CNetwork network( filename );

	const size_t traind_data_count = 50;

	for( unsigned int file_i = 0 ; file_i < traind_data_count ; file_i++ )
	{
		TrainingData test_input;
		getTestTrainData( test_input, file_i, traind_data_count );



		vector<double> output_test;
		network.Test( test_input.input, output_test );
		dump( output_test );

		printf( "expected=%.5f, out=%.5f\n", test_input.output[0], output_test[0] ); fflush( stdout );

		Graph::getInstance()->addPoint( test_input.input[0] * 3.0 * M_PI, output_test[0] * 2.0 - 1.0 );
	}

	Graph::getInstance()->drawPoints();
}


static void epoch_cb(EpochState & epochState)
{
	if( (epochState.index % 1000) == 0 )
	{
		printf("epochState.squareErrorSum=%.2f\n", epochState.squareErrorSum);fflush(stdout);
		Graph::getInstance()->clearPoints();
		Graph::getInstance()->addPoint( epochState.index * 0.01, epochState.squareErrorSum );
		Graph::getInstance()->drawPoints();
		printf("epochIndex=%d\n", epochState.index);fflush(stdout);
		string filename = "network_sin.net";
		epochState.network->save( filename );
		test_network_by_sin();
	}
}


void getTestTrainData( TrainingData & data, int index, int size )
{
	const double angleMin = 0;
	const double angleMax = 2 * M_PI;
	const double angleStep = ( angleMax - angleMin ) / size;

	double angle = angleStep * index;

	double normalizedAngle = angle / ( angleMax - angleMin );

	data.input.push_back( normalizedAngle );
	double value = ( sin( angle ) );
	data.output.push_back( value );
//	data.output.push_back( sin( angle ) );
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


