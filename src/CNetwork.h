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
#include <stdint.h>
#include <lbfgs.h>

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
	CNetwork * network;
	double squareErrorSum;
	int index;
}EpochState;

typedef struct
{
	CNetwork *lpThis;
	TrainingData *trainData;
}LBFGSEvaluateData;

typedef void (*EpochStateCallback)(EpochState & epochState);

class CNetwork
{
public:
	CNetwork( CLayersConfiguration & sequence, double learningRateStart = 1, double momentRate = 0.1 );
	CNetwork( string & filename );
	~CNetwork();

	double Learn( vector<TrainingData> & trainData, double error_threshold, unsigned int max_epoch );
	double LearnLBFGS( vector<TrainingData> & trainData, double error_threshold, unsigned int max_epoch );
	double Test( vector<double> & input, vector<double> & output_result );
	double Test( vector<double> & input, vector<double> & output_result, uint32_t reserved );
	bool save( string & filename );
	bool load( string & filename );
	void setEpochStateCallback( EpochStateCallback epochStateCallback );
	void reverse();
private:
	void createNetwork();
	void createNetwork( int32_t reserved );
	double forward( vector<double> & input, vector<double> & outputData );
	double forward( vector<double> & inputData, vector<double> & outputData, uint32_t reserved );
	void backpropagation2();
	void backpropagation2( uint32_t reserved );
	void backpropagation2( uint32_t reserved, uint32_t reserved2 );

	static lbfgsfloatval_t evaluate( void *instance, const lbfgsfloatval_t *x, lbfgsfloatval_t *g, const int n, const lbfgsfloatval_t step );

	void printInputLayer( CNetwork & network );

	bool m_fromFile;
	double m_LearningRate;
	double m_MomentRate;
	CLayersConfiguration * m_LayersConfiguration;
	EpochStateCallback m_epochStateCallback;

	lbfgs_parameter_t m_LbfgsParam;
	lbfgsfloatval_t *m_X;
	uint32_t m_weightsCount;
};

#endif /* CNETWORK_H_ */
