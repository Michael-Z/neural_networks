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

struct TrainingData
{
	vector<double> input;
	vector<double> output;
};

class CNetwork;

typedef struct
{
	CLayersConfiguration &layerConfiguration;
	CNetwork & network;
	double squareErrorSum;
	int index;
}EpochState;

typedef void (*EpochStateCallback)(EpochState & epochState);

class CNetwork
{
public:
	CNetwork( CLayersConfiguration & sequence, double learningRateStart = 1, double momentRate = 0.1 );
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
	double m_MomentRate;
	CLayersConfiguration * m_LayersConfiguration;
	EpochStateCallback m_epochStateCallback;
};

#endif /* CNETWORK_H_ */
