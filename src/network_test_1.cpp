//============================================================================
// Name        : network_test_1.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "network_by_mnist_images.h"
#include "network_by_images.h"
#include "network_by_and.h"
#include "network_by_numeric_prediction_1.h"
#include <iostream>
using namespace std;


int main()
{
	train_network_by_mnist_images();

//	train_network_by_images();
//	test_network_by_images();

//	train_network_by_and();
//	test_network_by_and();

//	train_network_by_numeric_prediction_1();
//	test_network_by_numeric_prediction_1();

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}




