/*
 * network_by_images.cpp
 *
 *  Created on: Jan 28, 2015
 *      Author: vostanin
 */

#include "network_by_images.h"
#include <cstdio>
#include <ctime>
#include <iostream>
#include <thread>

#include "Window.h"

static void getInputResolution( size_t & width, size_t & height );
static void getInputData( vector<double> & data, const char * file_format, int index );
static void dump( vector<double> & input, unsigned int items_per_line  );
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


void train_network_by_images()
{

	size_t width = 0, height = 0;
	getInputResolution( width, height );
	const unsigned int output_count = 26;
	const unsigned int input_count = width * height;
	vector<size_t> hiddenLayers;
	hiddenLayers.push_back( input_count / 2 );
	CLayersConfiguration sequence( input_count, output_count, hiddenLayers );

	CNetwork network( sequence, 10, 0.1 );

	string network_filename( "network_images.net" );

	vector<TrainingData> trainData_v;

	trainData_v.resize( output_count, TrainingData() );

	for( unsigned int file_i = 0 ; file_i < output_count ; file_i++ )
	{
		trainData_v[file_i].output.resize( output_count, 0.0 );
		getInputData( trainData_v[file_i].input, "./train_img/fangtasia-upper_%d.png_copy.png", file_i );

		vector<double>( trainData_v[file_i].output.size(), 0.0 ).swap( trainData_v[file_i].output );
		trainData_v[file_i].output[file_i] = 1;
	}

	double error_threshold = 1e-5;

	network.setEpochStateCallback( epoch_cb );

	double relativeErrorTrain = 0.0;
	{
		TIMER("train")

		relativeErrorTrain = network.Learn( trainData_v, error_threshold, 1000 );
	}
	printf( "relativeErrorTrain=%f\n", relativeErrorTrain ); fflush( stdout );

	network.save( network_filename );

	vector<double> test_input;
	getInputData( test_input, "./train_img/fangtasia-upper_%d.png_copy.png", 0 );


	vector<double> output_test;
	double relativeErrorTest = network.Test( test_input, output_test );
//	dump( output_test, 72 );
	printf( "relativeErrorTest=%f\n", relativeErrorTest ); fflush( stdout );

	for( unsigned int file_i = 0 ; file_i < output_count ; file_i++ )
	{
		getInputData( test_input, "./train_img/fangtasia-upper_%d.png_copy.png", file_i );

		vector<double> output_test;
		double relativeErrorTest = network.Test( test_input, output_test );
//		dump( output_test, 72 );
		size_t max_index = 0;
		double max_value = 0.0;
		for( size_t output_i = 0 ; output_i < output_test.size() ; output_i++ )
		{
			if( max_value < output_test[output_i] )
			{
				max_value = output_test[output_i];
				max_index = output_i;
			}
		}

		printf( "relativeErrorTest=%f, image_index=%d\n", relativeErrorTest, (int)max_index ); fflush( stdout );
	}
}

void test_network_by_images()
{
	size_t width = 0, height = 0;
	getInputResolution( width, height );
	const unsigned int output_count = 26;

	string filename = "network_images.net";

	CNetwork network( filename );

	for( unsigned int file_i = 0 ; file_i < output_count ; file_i++ )
	{
		vector<double> test_input;

		getInputData( test_input, "./train_img/fangtasia-upper_%d.png_copy.png", file_i );
//		dump( test_input, 72 );

		vector<double> output_test;
		double relativeErrorTest = network.Test( test_input, output_test );
//		dump( output_test, 72 );
		size_t max_index = 0;
		double max_value = 0.0;
		for( size_t output_i = 0 ; output_i < output_test.size() ; output_i++ )
		{
			if( max_value < output_test[output_i] )
			{
				max_value = output_test[output_i];
				max_index = output_i;
			}
		}

		printf( "relativeErrorTest=%f, image_index=%d, max_value=%f, expected_index=%d, expected_index_value=%f\n", relativeErrorTest, (int)max_index, max_value, file_i, output_test[file_i] ); fflush( stdout );
	}
}
#include "Graph.h"
static void epoch_cb(EpochState & epochState)
{
	if( (epochState.index % 10) == 0 )
	{
		Graph::getInstance()->addPoint( epochState.index * 0.01, epochState.squareErrorSum );
		Graph::getInstance()->drawPoints();
		printf("epochIndex=%d\n", epochState.index);fflush(stdout);
		string filename = "network_images.net";
//		epochState.network->save( filename );
//		test_network_by_images();
	}
}

#include "png++/png.hpp"
#include "fstream"

void getInputResolution( size_t & width, size_t & height )
{
	width = 0;
	height = 0;
	const char * file_name = "./train_img/fangtasia-upper_0.png_copy.png";

	ifstream png_file( file_name );
	if( true == png_file.is_open() )
	{
		png::reader<ifstream> reader(png_file);
		reader.read_info();
		height = reader.get_height();
		width = reader.get_width();

		png_file.close();
	}
}
void getInputData( vector<double> & data, const char * file_format, int index )
{
	const char * file_name_format = file_format;//"/home/vostanin/alhabit/fangtasia-upper_%d.png";
	char file_name[BUFSIZ] = { 0 };
	sprintf( file_name, file_name_format, index );

	ifstream png_file( file_name );
	data.clear();
	if( true == png_file.is_open() )
	{
		png::reader<ifstream> reader(png_file);
		reader.read_info();
		size_t height = reader.get_height();
		size_t width = reader.get_width();

		data.resize( height * width, 0.0 );

		for( size_t row_i = 0 ; row_i < height ; row_i++ )
		{
			png::byte * row = new png::byte[width];

			reader.read_row( row );
			for( size_t column_i = 0 ; column_i < width ; column_i++ )
			{
				int temp = (int)row[column_i];
				temp = abs( temp - 255 );//black color is significant
/*//first approach to represent input data
				if( temp < 2 )
				{
					temp = 2;
				}
				data[row_i*width + column_i] = 1.0 / temp;//one image failed result
*/
				//second approach to represent input data
				data[row_i*width + column_i] = temp / 255.0;//axcelent result
			}
		}
	}
}

static void dump( vector<double> & input, unsigned int items_per_line )
{
	ofstream file( "dump.txt", ios_base::app );

	for( size_t i = 0 ; i < input.size() ; i++ )
	{
		if( i % items_per_line == 0 )
		{
			file << endl;
		}
		file << input[i] << " " << std::flush;
	}
	file << endl;
	file.close();
}

