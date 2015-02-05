/*
 * CNetwork.h
 *
 *  Created on: Jan 26, 2015
 *      Author: vostanin
 */

#ifndef CNETWORK_H_
#define CNETWORK_H_

#include "CLayersConfiguration.h"
#include "ILayer.h"
#include <vector>
#include <string>
using namespace std;

#define BIASOUTPUT 1   //output for bias. It's always 1.
#define ETA 0.01   //learning rate

struct TrainingData
{
	vector<double> input;
	vector<double> output;
};

class CNetwork
{
public:
	CNetwork( CLayersConfiguration & sequence );
	CNetwork( string & filename );
	~CNetwork();

	double Learn( vector<TrainingData> & trainData, double error_threshold, unsigned int max_epoch );
	double Test( vector<double> & input, vector<double> & output_result );
	bool save( string & filename );
	bool load( string & filename );
private:
	void createNetwork();
	double forward( vector<double> & input, vector<double> & outputData );
	void backpropagation();

	bool m_fromFile;
	const double m_LearningRate;
	CLayersConfiguration * m_LayersConfiguration;
};

#endif /* CNETWORK_H_ */
