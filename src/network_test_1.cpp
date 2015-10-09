//============================================================================
// Name        : network_test_1.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "network_by_mnist_images_visual.h"
#include "network_by_and_visual.h"
#include "network_by_mnist_images.h"
#include "network_by_images.h"
#include "network_by_and.h"
#include "network_by_sin.h"
#include "network_by_numeric_prediction_1.h"
#include "network_by_dreams.h"
#include "Window.h"
#include "Graph.h"
#include <iostream>
#include <GraphicsMagick/Magick++.h>
using namespace std;

void* graph_tread( void * lp )
{
	Graph::getInstance( Window::getInstance().getWindow(), 50, 50 );
	Window::getInstance().run();

	return lp;
}

int main()
{
	Magick::InitializeMagick("");
//	train_network_by_mnist_images_visual();
//	train_network_by_and_visual();

//	train_network_by_and_visual();
//	test_network_by_and_visual();


//	test_network_reversed_by_mnist_images();

	pthread_t id = 0;
	pthread_create( &id, NULL, graph_tread, NULL );
	sleep( 1 );

//	train_network_by_mnist_images();
//	test_network_by_mnist_images();

//	train_network_by_images();

	train_network_by_and();
//	test_network_by_and();

//	train_network_by_sin();
//	test_network_by_sin();

//	test_network_reverse();

// //	train_network_by_numeric_prediction_1();
// //	test_network_by_numeric_prediction_1();

//	train_network_by_dreams();
//	test_network_by_dreams();

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	getchar();
	return 0;
}




