/*
 * network_by_images.cpp
 *
 *  Created on: Jan 28, 2015
 *      Author: vostanin
 */

#include "network_by_images.h"
#include <cstdio>
#include <sstream>
#include <dirent.h>
#include <unistd.h>
#include <GL/glut.h>
#include <cmath>

static void getInputResolution( size_t & width, size_t & height );
static void getInputData( vector<vector<double> > & data, int label );
static void getInputData( vector<double> & data, const char * file_format );
static void dump( vector<double> & input, unsigned int items_per_line  );
static void train_network_by_mnist_images();

static void initGraphics();
static void specialKeys( int key, int x, int y );
static void keyboardKeys( unsigned char symbol, int x, int y );
static void * thread_learn_network( void * userData );
static void epoch_cb(EpochState & epochState);

typedef union
{
	int color;
	GLbyte rgba[4];
}Color_T;

typedef struct
{
	double x, y, z;
	int color;
}ThreeDPoint;

static void object_file_save( vector<ThreeDPoint> & points );
static void object_file_save( ThreeDPoint & point );

static vector<ThreeDPoint> points;

static double rotate_x=-200;
static double rotate_y=-150;
static double rotate_z=-400;

static double move_y=-4;
static double move_x=-38;
static double move_z=0;

static double scale = 30.0;

void train_network_by_mnist_images_visual()
{
	thread_learn_network( NULL );
//	pthread_t thread_id = 0;
//	pthread_create( &thread_id, NULL, thread_learn_network, NULL );
//
//	initGraphics();
}

static void train_network_by_mnist_images()
{
	size_t width = 0, height = 0;
	getInputResolution( width, height );
	const unsigned int output_count = 10;
	const unsigned int input_count = width * height;
	vector<size_t> hiddenLayers;
	hiddenLayers.push_back( input_count / 2 );
	CLayersConfiguration sequence( input_count, output_count, hiddenLayers );

	CNetwork network( sequence, 1, 0.1 );
	network.setEpochStateCallback( epoch_cb );

	string network_filename( "network_mnist_images_visual.net" );

	vector<TrainingData> trainData_v;

	for( unsigned int label_i = 0 ; label_i < output_count ; label_i++ )
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

static void epoch_cb(EpochState & epochState)
{
	vector<ILayer*> layers;
	epochState.layerConfiguration.getLayers( layers );
	size_t layers_count = layers.size();

	static unsigned int color = 0xffffff;

	ThreeDPoint point;
	for(size_t layer_i = 1 ; layer_i < layers_count ; layer_i++ )
	{
		ILayer * layer = layers[layer_i];

		size_t neuronsCount = layer->getNeuronsCount();

		double distance = 0.0;
		double weights_square_sum = 0.0;

		for( size_t neuron_i = 0 ; neuron_i < neuronsCount ; neuron_i++ )
		{
			INeuron * neuron = layer->getNeuron( neuron_i );
			vector<Weight> & weights = neuron->getWeights();
			size_t weights_count = weights.size();
			for( size_t weight_i = 0 ; weight_i < weights_count ; weight_i++ )
			{
				double weight = weights[weight_i].value();
				weights_square_sum += weight * weight;
			}
		}

		distance = sqrt( weights_square_sum );

		if( layer_i == 1 )
		{
			point.x = distance;
		}
		else if( layer_i == 2 )
		{
			point.y = distance;
		}
	}

	color = color - 1;

	point.z = epochState.squareErrorSum;

	point.color = color;

	points.push_back( point );

	object_file_save( points );

	usleep(1000);
}

static void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-20, 20, -20, 20, 20, -20);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef( (-1)*move_x, move_y, 0.0 );

	glRotatef( rotate_x, 1.0, 0.0, 0.0 );
	glRotatef( rotate_y, 0.0, 1.0, 0.0 );
	glRotatef( rotate_z, 0.0, 0.0, 1.0 );

	glScalef( scale, scale, scale );


	glBegin(GL_TRIANGLES);
	size_t points_count = points.size();
	for( size_t point_i = 0 ; point_i < points_count ; point_i++ )
	{
		ThreeDPoint & point = points[point_i];
		glColor3bv( (GLbyte*)&point.color );
		glVertex3f( point.x, point.y, point.z);
	}
	glEnd();

	glFlush();

	glutSwapBuffers();
}

static void * thread_learn_network( void * userData )
{
	train_network_by_mnist_images();

	object_file_save( points );

	return NULL;
}

static void initGraphics()
{
	char ** argv = (char**)malloc(sizeof(char*));
	argv[0] = "neural_networks";
	int argc = 1;
	glutInit( &argc, argv );
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Hello world");
	glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glClearColor(0.5, 0.5, 0.5, 0);
	glutSpecialFunc(specialKeys);
	glutKeyboardFunc( keyboardKeys );

	glutMainLoop();
}

static void keyboardKeys( unsigned char symbol, int x, int y )
{
	switch (symbol)
	{
	case '+':
		scale += 0.1;
		scale += 0.1;
		scale += 0.1;
		break;
	case '-':
		scale -= 0.1;
		scale -= 0.1;
		scale -= 0.1;
		break;
	case 'a':
		move_x -= 0.1;
		break;
	case 'd':
		move_x += 0.1;
		break;
	case 'w':
		move_y -= 0.1;
		break;
	case 's':
		move_y += 0.1;
		break;
	}
}
/**** special key***/
static void specialKeys( int key, int x, int y )
{
	if (key == GLUT_KEY_RIGHT)
	{
		rotate_y += 5;
	}
	else if (key == GLUT_KEY_LEFT)
	{
		rotate_y -= 5;
	}
	else if (key == GLUT_KEY_UP)
	{
		rotate_x += 5;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		rotate_x -= 5;
	}
	else if (key == GLUT_KEY_PAGE_DOWN)
	{
		rotate_z -= 5;
	}
	else if (key == GLUT_KEY_PAGE_UP)
	{
		rotate_z += 5;
	}
	glutPostRedisplay();
}

static void test_network_by_mnist_images()
{
	size_t width = 0, height = 0;
	getInputResolution( width, height );
	const unsigned int output_count = 10;

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

static void object_file_save( ThreeDPoint & point )
{
	FILE *pfile = fopen("graph.obj", "r");
	bool file_exists = false;
	if( NULL != pfile)
	{
		fclose( pfile );
		file_exists = true;
	}

	ofstream file("graph.obj", ios_base::app);
	if( false == file.is_open() )
	{
		return;
	}

	if( false == file_exists)
	{
		file << "o Pilot.001" << endl << flush;
	}

	file << "v " << point.x << " " << point.z << " " << point.y << endl;
}

static void object_file_save( vector<ThreeDPoint> & point )
{
	FILE *pfile = fopen("graph.obj", "r");
	bool file_exists = false;
	if( NULL != pfile)
	{
		fclose( pfile );
		file_exists = true;
	}

	ofstream file("graph.obj", ios_base::app);
	if( false == file.is_open() )
	{
		return;
	}

	if( false == file_exists)
	{
		file << "o Pilot.001" << endl << flush;
	}

	size_t points_count = points.size();
	for( size_t point_i = 0 ; point_i < points_count ; point_i++ )
	{
		ThreeDPoint & point = points[point_i];
		file << "v " << point.x << " " << point.z << " " << point.y << endl;
	}

//	file << "usemtl None" << endl;
//
//	if( points_count > 5 )
//	{
//		for( size_t point_i = 1 ; point_i < ( points_count - 5 ) ; point_i++ )
//		{
//			file << "f " << point_i << " " << point_i + 1 << " " << point_i + 2 << " " << point_i + 3 << endl;
//		}
//	}

}
