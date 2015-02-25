/*
 * network_by_and.cpp
 *
 *  Created on: Jan 28, 2015
 *      Author: vostanin
 */

#include "network_by_and_visual.h"
#include <cstdio>
#include <cmath>
#include <GL/glut.h>
#include <unistd.h>
#include <fstream>

static void getTestTrainData( TrainingData & data, int index );
static void dump( vector<double> & input );
static void epoch_cb(EpochState& epochState);
static void train_network_by_and();

static void initGraphics();
static void specialKeys( int key, int x, int y );
static void keyboardKeys( unsigned char symbol, int x, int y );
static void * thread_learn_network( void * userData );

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

static void object_file_save( ThreeDPoint & point );

static vector<ThreeDPoint> points;

static double rotate_x=-200;
static double rotate_y=-150;
static double rotate_z=-400;

static double move_y=-4;
static double move_x=-38;
static double move_z=0;

static double scale = 30.0;

void train_network_by_and_visual()
{
	thread_learn_network(NULL);
//	pthread_t thread_id = 0;
//	pthread_create( &thread_id, NULL, thread_learn_network, NULL );

//	initGraphics();
}

static void train_network_by_and()
{
	const unsigned int input_count = 2;
	const unsigned int output_count = 1;
	vector<size_t> hiddenLayers;
	hiddenLayers.push_back( 2 );
	CLayersConfiguration sequence( input_count, output_count, hiddenLayers );



	string network_filename( "network_and_visual.net" );

	vector<TrainingData> trainData_v;

	const size_t traind_data_count = 4;

	trainData_v.resize( traind_data_count, TrainingData() );

	for( unsigned int file_i = 0 ; file_i < traind_data_count ; file_i++ )
	{
		getTestTrainData( trainData_v[file_i], file_i );
	}

	double error_threshold = 1e-5;

	size_t network_count = 500;

	for( size_t network_i = 0 ; network_i < network_count ; network_i++ )
	{
		CNetwork network( sequence );
		network.setEpochStateCallback( epoch_cb );
		double relativeErrorTrain = network.Learn( trainData_v, error_threshold, 30000 );
		printf( "relativeErrorTrain=%f\n", relativeErrorTrain ); fflush( stdout );
	}
//	network.save( network_filename );

//	for( unsigned int file_i = 0 ; file_i < traind_data_count ; file_i++ )
//	{
//		TrainingData test_input;
//		getTestTrainData( test_input, file_i );
//
//		vector<double> output_test;
//		double relativeErrorTest = network.Test( test_input.input, output_test );
//		printf( "relativeErrorTest=%f\n", relativeErrorTest ); fflush( stdout );
//		dump( output_test );
//		double max_value = 0.0;
//		for( size_t output_i = 0 ; output_i < output_test.size() ; output_i++ )
//		{
//			if( max_value < output_test[output_i] )
//			{
//				max_value = output_test[output_i];
//			}
//		}
//
//		printf( "relativeErrorTest=%f, value=%d\n", relativeErrorTest, (int)max_value ); fflush( stdout );
//	}
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
			vector<double> weights;
			neuron->getWeights( weights );
			size_t weights_count = weights.size();
			for( size_t weight_i = 0 ; weight_i < weights_count ; weight_i++ )
			{
				double weight = weights[weight_i];
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

	object_file_save( point );
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
	train_network_by_and();

//	object_file_save( points );

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

void test_network_by_and_visual()
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

		printf( "relativeErrorTest=%f, image_index=%d\n", relativeErrorTest, (int)max_value ); fflush( stdout );
	}
}

static void getTestTrainData( TrainingData & data, int index )
{
	static int all_input_data[][3] = { {0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 1} };

	data.input.resize( 2, 0 );
	data.input[0] = all_input_data[index][0];
	data.input[1] = all_input_data[index][1];

	data.output.push_back( all_input_data[index][2] );
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

//	size_t points_count = points.size();
//	for( size_t point_i = 0 ; point_i < points_count ; point_i++ )
	{
//		ThreeDPoint & point = points[point_i];
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

