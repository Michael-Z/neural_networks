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
#define ETA 0.7   //learning rate

struct TrainingData
{
	vector<double> input;
	vector<double> output;
};

typedef struct
{
	CLayersConfiguration &layerConfiguration;
	double squareErrorSum;
}EpochState;

typedef void (*EpochStateCallback)(EpochState & epochState);

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
	void setEpochStateCallback( EpochStateCallback epochStateCallback );
	void setLearnRate( double learnRate );
	void reverse();
private:
	void createNetwork();
	double forward( vector<double> & input, vector<double> & outputData );
	void backpropagation();
	void backpropagation2();

	bool m_fromFile;
	double m_LearningRate;
	CLayersConfiguration * m_LayersConfiguration;
	EpochStateCallback m_epochStateCallback;
};

#endif /* CNETWORK_H_ */
