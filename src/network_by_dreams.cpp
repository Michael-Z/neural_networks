/*
 * network_by_and.cpp
 *
 *  Created on: Jan 28, 2015
 *      Author: vostanin
 */

#include "network_by_and.h"
#include <GraphicsMagick/Magick++.h>
#include <cstdio>
#include <iostream>
#include <ctime>
using namespace std;

static void getTestTrainData( TrainingData & data, int index );
static void dump( vector<double> & input );
static void epoch_cb(EpochState & epochState);
static void writeimage( uint32_t index, vector<uint8_t> & colors );

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

uint32_t image_width = 0;
uint32_t image_height = 0;

void train_network_by_dreams()
{
	{
		Magick::Image image( "/cryptfs/workspace_cpp/neural_networks/bird.png" );
		image_width = image.columns();
		image_height = image.rows();
	}

	const unsigned int input_count = image_width * image_height;
	const unsigned int output_count = image_width * image_height;
	vector<size_t> hiddenLayers;

//	hiddenLayers.push_back( 100 );
	CLayersConfiguration sequence( input_count, output_count, hiddenLayers );

	CNetwork network( sequence, 1, 0.1 );

	string network_filename( "network_dreams.net" );

	vector<TrainingData> trainData_v;

	const size_t traind_data_count = 1;

	trainData_v.resize( traind_data_count, TrainingData() );

	getTestTrainData( trainData_v[0], 0 );

	double error_threshold = 1e-7;

	network.setEpochStateCallback( epoch_cb );

	double relativeErrorTrain = 0.0;
	{
		TIMER("train")

		relativeErrorTrain = network.Learn( trainData_v, error_threshold, 1000000 );
	}
	printf( "relativeErrorTrain=%f\n", relativeErrorTrain ); fflush( stdout );
	network.save( network_filename );

	for( unsigned int file_i = 0 ; file_i < traind_data_count ; file_i++ )
	{
		TrainingData test_input;
		getTestTrainData( test_input, file_i );

		vector<double> output_test;
		double relativeErrorTest = network.Test( test_input.input, output_test );
		dump( output_test );
		double max_value = 0.0;
		for( size_t output_i = 0 ; output_i < output_test.size() ; output_i++ )
		{
			if( max_value < output_test[output_i] )
			{
				max_value = output_test[output_i];
			}
		}

		printf( "relativeErrorTest=%f, value=%f\n", relativeErrorTest, max_value ); fflush( stdout );
	}
}

#include "Graph.h"
static void epoch_cb(EpochState & epochState)
{
	if( (epochState.index % 1) == 0 )
	{
		Graph::getInstance()->addPoint( epochState.index * 0.01, epochState.squareErrorSum );
		Graph::getInstance()->drawPoints();
		printf("epochIndex=%d\n", epochState.index);fflush(stdout);
		string filename = "network_images.net";
		epochState.network->save( filename );
		vector<ILayer*> layers;
		vector<uint8_t> colors;
		epochState.layerConfiguration.getLayers( layers );
		uint32_t count = layers[ layers.size() - 1 ]->getNeuronsCount();
		colors.resize( count );
		for( uint32_t output_i = 0 ; output_i < count ; output_i++ )
		{
			colors[output_i] = layers[ layers.size() - 1 ]->getNeuron( output_i )->getOutput() * 255.0;
		}
		writeimage( epochState.index, colors );
//		test_network_by_dreams();
	}
}

void test_network_by_dreams()
{
	const unsigned int input_count = 2;
	const unsigned int output_count = 1;
	vector<size_t> hiddenLayers;
	CLayersConfiguration sequence( input_count, output_count, hiddenLayers );

	string filename = "network_and.net";
	CNetwork network( filename );

	const size_t traind_data_count = 4;

	for( unsigned int file_i = 0 ; file_i < traind_data_count ; file_i++ )
	{
		TrainingData test_input;
		getTestTrainData( test_input, file_i );

		vector<double> output_test;
		network.Test( test_input.input, output_test );
		dump( output_test );
		double max_value = 0.0;

		printf( "out=%f\n", output_test[0] ); fflush( stdout );
	}
}

void getTestTrainData( TrainingData & data, int index )
{
	Magick::Image image( "/cryptfs/workspace_cpp/neural_networks/bird.png" );
	uint32_t image_width = image.rows();
	uint32_t image_height = image.columns();

	data.input.resize( image_width * image_height, 0 );
	data.output.resize( image_width * image_height, 0 );

	const Magick::PixelPacket* pixels = image.getConstPixels( 0, 0, image_width, image_height );

	for( uint32_t pixel_i = 0 ; pixel_i < image_width * image_height ; pixel_i++, pixels++ )
	{
		double r = pixels->red / 255.0 * 0.98 + 0.01;
//		uint8_t g = pixels->green;
//		uint8_t b = pixels->blue;

		data.input[pixel_i] = r;//gray image
		data.output[pixel_i] = r;//gray image
	}
}

#include <fstream>

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

void writeimage( uint32_t index, vector<uint8_t> & colors )
{
	char path[BUFSIZ] = { '\0' };
	sprintf( path, "./image_0.png", index );
	Magick::Image image;
	image.size( Magick::Geometry( image_width, image_height ) );

	Magick::PixelPacket* pixels = image.getPixels( 0, 0, image_width, image_height );

	for( uint32_t pixel_i = 0 ; pixel_i < image.columns() * image.rows() ; pixel_i++, pixels++ )
	{
		pixels->red = colors[pixel_i];
		pixels->green = colors[pixel_i];
		pixels->blue = colors[pixel_i];
	}
	image.syncPixels();
	image.write( path );

}

