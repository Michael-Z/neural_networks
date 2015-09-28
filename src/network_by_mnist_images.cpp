/*
 * network_by_images.cpp
 *
 *  Created on: Jan 28, 2015
 *      Author: vostanin
 */

#include "network_by_mnist_images.h"
#include <cstdio>
#include <sstream>
#include <dirent.h>

static void getInputResolution( size_t & width, size_t & height );
static void getInputData( vector<vector<double> > & data, int label );
static void getInputData( vector<double> & data, const char * file_format );
static void dump( vector<double> & input, unsigned int items_per_line  );
static void epoch_cb(EpochState & epochState);

void train_network_by_mnist_images()
{
	size_t width = 0, height = 0;
	getInputResolution( width, height );
	const unsigned int output_count = 2;
	const unsigned int input_count = width * height;
	vector<size_t> hiddenLayers;
	hiddenLayers.push_back( input_count / 2 );
	CLayersConfiguration sequence( input_count, output_count, hiddenLayers );

	CNetwork network( sequence, 1, 0.1 );

	network.setEpochStateCallback( epoch_cb );

	string network_filename( "network_mnist_images.net" );

	vector<TrainingData> trainData_v;

	for( unsigned int label_i = 0 ; label_i < 2 ; label_i++ )
	{
		vector<vector<double> > all_input_data;
		getInputData( all_input_data, label_i );

		size_t all_input_data_count = all_input_data.size();

		for( size_t input_i = 0 ; input_i < all_input_data_count ; input_i++ )
		{
			TrainingData train_data;
			train_data.input = all_input_data[input_i];

			train_data.output.resize( output_count, 0.0 );
			train_data.output[label_i] = 1;

			trainData_v.push_back( train_data );
		}
	}

	double error_threshold = 1e-4;

	double relativeErrorTrain = network.Learn( trainData_v, error_threshold, 300 );
	printf( "relativeErrorTrain=%f\n", relativeErrorTrain ); fflush( stdout );

	network.save( network_filename );


	vector<double> output_test;
	for( unsigned int label_i = 0 ; label_i < output_count ; label_i++ )
	{
		vector<vector<double> > inputData_v;

		getInputData( inputData_v, label_i );

		vector<double> output_test;
		size_t inputData_v_count = inputData_v.size();
		for( size_t image_i = 0 ; image_i < inputData_v_count ; image_i++ )
		{
			double relativeErrorTest = network.Test( inputData_v[image_i], output_test );
			dump( output_test, 72 );
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
}

#include "Graph.h"
static void epoch_cb(EpochState & epochState)
{
	if( (epochState.index % 1) == 0 )
	{
		Graph::getInstance()->addPoint( epochState.index * 0.01, epochState.squareErrorSum );
		Graph::getInstance()->drawPoints();
		printf("epochIndex=%d\n", epochState.index);fflush(stdout);
		string filename = "network_mnist_images.net";
		epochState.network->save( filename );
		test_network_by_mnist_images();
	}
}

void test_network_by_mnist_images()
{
	const unsigned int output_count = 2;

	string filename = "network_mnist_images.net";
	CNetwork network( filename );

	vector<double> output_test;
	for( unsigned int label_i = 0 ; label_i < output_count ; label_i++ )
	{
		vector<vector<double> > inputData_v;

		getInputData( inputData_v, label_i );

		vector<double> output_test;
		size_t inputData_v_count = inputData_v.size();
		for( size_t image_i = 0 ; image_i < inputData_v_count ; image_i++ )
		{
			double relativeErrorTest = network.Test( inputData_v[image_i], output_test );
//			dump( output_test, 72 );
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
			printf( "image_index=%d expected=%d, maxvalue=%f\n", (int)max_index, label_i, max_value ); fflush( stdout );
		}
	}
}

#include "png++/png.hpp"
#include "fstream"

void getInputResolution( size_t & width, size_t & height )
{
	width = 0;
	height = 0;
	const char * file_name = "./mnist_train/0/image_1.png";

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

void getReversedInputData( vector<vector<double> > & all_inputs, int label )
{
	vector<double> input;
	input.resize( 10, 0 );
	input[ label ] = 1;

	all_inputs.push_back( input );
}

void getInputData( vector<vector<double> > & all_inputs, int label )
{
	stringstream dir_name_stream;
	dir_name_stream << "./mnist_train/" << (char)( 48 + label ) << flush;
	DIR * dir_handle = opendir( dir_name_stream.str().c_str() );
	if(NULL == dir_handle)
	{
		return;
	}
	struct dirent *dir_entry = NULL;
	do
	{
		dir_entry = readdir(dir_handle);
		if (NULL == dir_entry)
		{
			break;
		}
		if(0 == strcmp(dir_entry->d_name, ".") || 0 == strcmp(dir_entry->d_name, ".."))
		{
			continue;
		}
		char file_path[PATH_MAX] = {'\0'};
		sprintf(file_path, "%s/%s", dir_name_stream.str().c_str(), dir_entry->d_name);

		vector<double> input;

		getInputData( input, file_path );

		all_inputs.push_back( input );

	}while(true);
	closedir(dir_handle);
}

void savePng( string filename, vector<double> data, int width, int height )
{
	ofstream png_file( filename.c_str() );
	if( true == png_file.is_open() )
	{
		png::writer<ofstream> writer(png_file);

		writer.set_height( height );
		writer.set_width( width );
		writer.set_color_type( png::color_type_gray );
		writer.set_bit_depth( 8 );
		writer.write_info();
		png::byte * row = new png::byte[width];
		for( int row_i = 0 ; row_i < height ; row_i++ )
		{

			for( int col_i = 0 ; col_i < width ; col_i++ )
			{
				row[col_i] = data[row_i * width + col_i ] * 255;
			}
			writer.write_row( row );
		}

		png_file.close();
	}
}

void getInputData( vector<double> & data, const char * file_name )
{
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
//				temp = abs( temp - 255 );//black color is significant
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

void test_network_reversed_by_mnist_images()
{
	string filename = "network_mnist_images.net";
	CNetwork network( filename );

	filename = "network_mnist_images_reversed.net";
	network.reverse();
	network.save( filename );

	size_t width = 0, height = 0;
	getInputResolution( width, height );

	unsigned int test_samples_count = 10;

	const unsigned int output_count = width * height;
	const unsigned int input_count = test_samples_count;

	vector<double> output_test;
	for( unsigned int label_i = 0 ; label_i < test_samples_count ; label_i++ )
	{
		vector<vector<double> > inputData_v;

		getReversedInputData( inputData_v, label_i );

		vector<double> output_test;
		size_t inputData_v_count = inputData_v.size();
		for( size_t image_i = 0 ; image_i < inputData_v_count ; image_i++ )
		{
			double relativeErrorTest = network.Test( inputData_v[image_i], output_test );

			stringstream sstr;
			sstr << "./reverse_mnist_image_" << label_i << ".png" << flush;
			savePng( sstr.str(), output_test, width, height );
		}
	}
}
