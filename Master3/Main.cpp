#include "GraphWindow.h"
#include "MainDlg.h"
#include "Simulation.h"
#include "Candlestick.h"
#include "CandlestickSeries.h"
#include "AnnWindow.h"

#include <xercesc/util/PlatformUtils.hpp>

// application entry point

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, INT iShowCmd)
{
	xercesc::XMLPlatformUtils::Initialize() ;

	MainDlg::create(hInstance);

	Simulation *psim = Simulation::getSingleton() ;
	psim->setXmlFile( lpCmdLine ) ;

	MSG msg ;

	DWORD dwTime0 = GetTickCount() ;
	while(true) {
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) break ;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(!psim->initialized()) {
			try {
				psim->step() ;	// do the initialization
				GraphWindow::create(hInstance);
				AnnWindow::create(hInstance);
			} catch( SimException e ) {
				e.log() ;
				exit(-1) ;
			}
		}
		else if(psim->running()) {
			unsigned n=100;	// performing the simulation steps in batches greatly enhances performance
			while(n--) psim->step() ;

			if(GetTickCount()>(dwTime0+100) || !psim->running()) {
				// update gui
				MainDlg *mainDlg = MainDlg::getSingleton() ;
				GraphWindow *graphWnd = GraphWindow::getSingleton() ;
				AnnWindow *annWnd = AnnWindow::getSingleton() ;

				mainDlg->updateProgress() ;
				graphWnd->update() ;
				annWnd->update() ;

				dwTime0 = GetTickCount();
			}
		}
		else {
			WaitMessage() ;
		}
	}

	xercesc::XMLPlatformUtils::Terminate();

	return 0;
}
