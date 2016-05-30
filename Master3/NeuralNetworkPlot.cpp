#include "NeuralNetwork.h"
#include "SimException.h"
#include "Agent.h"
#include "boost/date_time/posix_time/posix_time.hpp"

void NeuralNetwork::drawPlot()
{
	// TODO
	if(!this->ann) return ;

	const unsigned layers = fann_get_num_layers(ann) ;
	unsigned *layer_neurons = new unsigned[layers] ;
	fann_get_layer_array(ann, layer_neurons) ;
//	fann_connection *connections = new fann_connection[ fann_get_total_connections(ann) ] ;
//	fann_get_connection_array(ann, connections);

	unsigned l_max = 0;
	for(unsigned l=0;l<layers;l++) {
		if( layer_neurons[l] > l_max ) l_max = layer_neurons[l] ;
	}

	unsigned neuron_rad = min( this->w / ( 2.5 * l_max ), this->h / ( 2.5 * layers) )  ;

	// paint the connections of the network

	const unsigned gridh = this->h / layers ;

	HBRUSH hOldBrush = (HBRUSH) SelectObject(hMemDC, GetStockObject( DKGRAY_BRUSH )) ;
	HPEN hOldPen = (HPEN) SelectObject(hMemDC, GetStockObject( WHITE_PEN )) ;
	unsigned n_cnt = 0 ;
	for(unsigned l=0;l<layers;l++) {
		const unsigned neurons = layer_neurons[l] ;

		RECT grid ;
		grid.top = l * this->h / layers ;
		grid.bottom = (l + 1) * this->h / layers ;

		for(unsigned n=0;n<neurons;n++) {
			grid.left = n * this->w / neurons ;
			grid.right = (n + 1) * this->w / neurons ;

			unsigned x = ( grid.left + grid.right ) / 2 ;
			unsigned y = ( grid.top + grid.bottom ) / 2 ;

			// draw synapses
			if(l != layers-1) {	// skip output layer
				unsigned y1 = (2*l+3)*this->h / (2*layers) ;
				for(unsigned m=0;m<layer_neurons[l+1];m++) {
					unsigned x1 = (2*m+1)*this->w / (2*layer_neurons[l+1]) ;
					MoveToEx(hMemDC, x, y, NULL) ;
					LineTo(hMemDC, x1, y1) ;
				}
			}

			Ellipse(hMemDC, x - neuron_rad, y - neuron_rad, x + neuron_rad, y + neuron_rad) ;

			std::wstring neuron_id = x2s(++n_cnt) ;
			DrawText(hMemDC, neuron_id.c_str(), neuron_id.length(), &grid, DT_SINGLELINE | DT_VCENTER | DT_CENTER) ;
		}
	}
	SelectObject(hMemDC, hOldPen);
	SelectObject(hMemDC, hOldBrush) ;
	
//	delete[] connections ;
	delete[] layer_neurons ;

	updateInfoText() ;
}

void NeuralNetwork::updateInfoText() {
	const unsigned layers = fann_get_num_layers(ann) ;
	const unsigned tot_con = fann_get_total_connections(ann) ;

	unsigned *layer_neurons = new unsigned[layers] ;

	fann_connection *connections = new fann_connection[tot_con];
	fann_get_layer_array(ann, layer_neurons) ;

	fann_layer *layer_it = ann->first_layer ;
	
	neuronText.clear() ;
	connectionText.clear() ;

	neuronText += L"  ==NEURONS==\r\n\r\n" ;
	connectionText += L"  ==CONNECTIONS==\r\n" ;

	unsigned cnt = 0;
	for(unsigned l=0;l<layers;l++) {
		fann_neuron *neuron_it = layer_it[l].first_neuron ;
		for(unsigned n=0;n<layer_neurons[l];n++) {
			neuronText += L"  #" + x2s(++cnt);

			if(!l) neuronText += L" [" + (*pagents)[n]->toString() + L"]";
			else if(l==layers-1) neuronText += L" [" + boost::posix_time::to_simple_wstring(durations[n]) + L"]\t\t";
			else neuronText += L"\t\t\t" ;

//			if(!!l) neuronText += L"\tsum=" + x2s( neuron_it[n].sum );
			neuronText += L"\tval=" + x2s( neuron_it[n].value );


			neuronText += L"\r\n" ;
		}
	}

	

	delete [] layer_neurons ;
	delete [] connections ;
}