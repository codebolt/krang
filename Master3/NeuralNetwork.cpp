#pragma warning( disable : 244 )	// annoying warning about converting wchar to char

#include "NeuralNetwork.h"
#include "Agent.h"
#include "Simulation.h"
#include "SimException.h"

// use xerces-c dom for xml
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

NeuralNetwork::NeuralNetwork(const std::vector<Agent*> &_agents, const TradeSeries *_ptrades, 
							 const std::vector<boost::posix_time::time_duration>& _durations, const std::wstring& _name, const std::vector<int> &_hiddenLayers,
							 bool _train)
		: Plot(true), pagents(&_agents), ptrades(_ptrades), durations(_durations), name(_name), ann(0), hiddenLayers(_hiddenLayers), train(_train)
{
	for(int i=0;i<hiddenLayers.size();i++) SIM_ASSERT(hiddenLayers.at(i) > 0) ;
	this->inputLayerSize = pagents->size() ;
	this->outputLayerSize = durations.size() ;
}

void NeuralNetwork::create()
{
	// create the network so its ready for training
	unsigned *layers = new unsigned[hiddenLayers.size() + 2] ;
	layers[0] = inputLayerSize ;
	for(int i=0;i<hiddenLayers.size();i++) layers[i+1] = hiddenLayers.at(i) ;
	layers[hiddenLayers.size()+1] = outputLayerSize ;
	this->ann = fann_create_standard_array(hiddenLayers.size()+2, layers) ;

    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_randomize_weights(ann, 0.005, 0.020) ;

	inputLayer = new fann_type[ inputLayerSize ] ;
	outputLayer = new fann_type[ outputLayerSize ] ;

	tradeIterator = ptrades->begin() ;
	delete[] layers ;
}

void NeuralNetwork::configtrain(double lrate, double lmom) {
	SIM_ASSERT(!!ann) ;

	fann_set_learning_rate(ann, lrate) ;
	fann_set_learning_momentum(ann, lmom) ;
}

void NeuralNetwork::save()
{
	std::wstring path = name + L".ann" ;
	std::string temp(path.length(), ' ') ;
	std::copy(path.begin(), path.end(), temp.begin()) ;
	int res = fann_save(ann, temp.c_str()) ;
	assert(res == 0) ;
}

void NeuralNetwork::load()
{
	std::wstring path = name + L".ann" ;
	std::string temp(path.length(), ' ') ;
	std::copy(path.begin(), path.end(), temp.begin()) ;

	this->ann = fann_create_from_file(temp.c_str()) ;

	// some simple test assertions to make sure the layer matches what we expect
	SIM_ASSERT(!!ann) ;
	SIM_ASSERT(fann_get_num_input(ann) == inputLayerSize) ;
	SIM_ASSERT(fann_get_num_output(ann) == outputLayerSize) ;

	inputLayer = new fann_type[ inputLayerSize ] ;
	outputLayer = new fann_type[ outputLayerSize ] ;

	tradeIterator = ptrades->begin() ;

	fann_reset_MSE(ann) ;

	if(!train) {
		for(int u=0;u<outputLayerSize;u++) {
			statLog.push_back(StatLogger(durations[u])) ;
		}
	}
}

void NeuralNetwork::update()
{
	SIM_ASSERT(!!ann) ;
	int activeAgents = 0 ;

	// prepare the input layer array
	for(int i=0;i<inputLayerSize;i++) {
		inputLayer[i] = (*pagents)[i]->getSignal() ;
		if( (*pagents)[i]->active() ) activeAgents ++ ;
	}

	const Candlestick& lastACS = Simulation::getSingleton()->lastAtomic() ;
	const boost::posix_time::ptime& pt = lastACS.tp.end() ;
	const double currPrice = lastACS.close ;

	//increment the trade iterator
	while((tradeIterator!=ptrades->end()) && (tradeIterator->time <= pt)) tradeIterator++ ;
	if(tradeIterator == ptrades->end()) return ;

	if(this->train) {
		// training mode
		// need to load the correct values into the output layer and run the training algo
		// prepare the output layer array
		for(int i=0;i<outputLayerSize;i++) {
			TradeSeries::ConstIterator tmpit = tradeIterator ;
			boost::posix_time::ptime t1 = pt + durations[i] ;
			while((tmpit != ptrades->end()) && (tmpit->time <= t1)) tmpit ++ ;
			if(tmpit == ptrades->end()) { tmpit -- ; }

			outputLayer[i] = (tmpit->price - currPrice) / currPrice ;	// yield
			assert( abs(outputLayer[i]) <= 1.0 ) ;
//			statLog.at(i).update(last.time, outputLayer[i], (last.price - tmpit->price) / last.price, last.price) ;
		}

		fann_test(ann, inputLayer, outputLayer) ;
		fann_train(ann, inputLayer, outputLayer) ;
	}
	else {
		// normal execution

		// run the network
		const fann_type *_outputLayer = fann_run(ann, inputLayer) ;

		// save a copy of the result
		for(int i=0;i<outputLayerSize;i++) {
			TradeSeries::ConstIterator tmpit = tradeIterator ;
			boost::posix_time::ptime t1 = pt + durations[i] ;
			while((tmpit != ptrades->end()) && (tmpit->time <= t1)) tmpit ++ ;
			if(tmpit == ptrades->end()) { tmpit -- ; }

			outputLayer[i] = _outputLayer[i] ;

			// log only if at least half the agents are active
			if(activeAgents >= (inputLayerSize/2))
				statLog.at(i).update(pt, outputLayer[i], (tmpit->price - currPrice) / currPrice, currPrice) ;
		}
	}
}

std::wstring NeuralNetwork::getStatText()
{ 
	std::wstring str = L"  ==STATISTICS==\r\n\r\n" ;

	if(!train) {
		for(int u=0;u<outputLayerSize;u++) {
			str += statLog.at(u).getDisplayText();
		}
	}
	else {
		// TODO
	}

	return str ;
}

std::wstring NeuralNetwork::getLogText()
{
	std::wstringstream ss ;

	if(!train) {
		ss << L"Testing complete.\r\n\r\n" 
		   << statLog.at(0).getFirstRow() << statLog.at(0).getStockRow() ;
		for(int u=0;u<outputLayerSize;u++) {
			ss << statLog.at(u).getRowText() ;
		}
	}
	else {
		ss << L"Training complete.\r\n\r\nMSE="
		   << fann_get_MSE(ann) ;
	}

	return ss.str() ;
}