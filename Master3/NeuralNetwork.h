#pragma once

#define FANN_NO_DLL

#include <doublefann.h>
#include <vector>

#include "boost/date_time/posix_time/posix_time_types.hpp"

#include "TradeSeries.h"
#include "Plot.h"
#include "StatLogger.h"

class Agent ;

class NeuralNetwork : public Plot {
	const std::vector<boost::posix_time::time_duration> durations ;

	const std::vector<Agent*> *pagents ;
	const TradeSeries *ptrades ;

	std::vector<StatLogger> statLog ;	// one for each output duration

	TradeSeries::ConstIterator tradeIterator ;
	fann* ann ;

	std::vector<int> hiddenLayers ;
	int inputLayerSize, outputLayerSize ;
	fann_type *inputLayer, *outputLayer ;

	std::wstring name ;
	std::wstring neuronText ;
	std::wstring connectionText ;

	bool train ;

	void updateInfoText() ;

public:
	NeuralNetwork(const std::vector<Agent*> &_agents, const TradeSeries *_ptrades, 
		const std::vector<boost::posix_time::time_duration>& _durations, const std::wstring& _name,
		const std::vector<int>& _hiddenLayers, bool _train) ;

	~NeuralNetwork() { } ;

	void save() ;	// save network
	void load() ;	// load network

	void configtrain(double lrate, double lmom) ;

	// simulation functions
	void create() ;	// before training
	void finalize() { if(train) save(); }	// save if necessary
	void update() ;	// train the network

	// plot functions
	void drawPlot() ;
	void setText(unsigned) { } ;

	std::wstring getNeuronText() { return neuronText; }
	std::wstring getConnectionText() { return connectionText; }
	std::wstring getStatText() ;
	std::wstring getLogText() ;

	friend class Simulation ;
};