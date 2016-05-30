#include "MainDlg.h"
#include "Simulation.h"

#include <mmsystem.h>
#include <commctrl.h>
#include <windowsx.h>
#include "resource.h"

MainDlg* MainDlg::singleton = 0 ;

INT_PTR CALLBACK MainDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) ;	// dispatches messages, defined at the bottom
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) ; // message handler for the about dialog


bool MainDlg::create(HINSTANCE hInstance) {
	MainDlg::singleton = new MainDlg();

	singleton->hDlg = CreateDialog( hInstance, MAKEINTRESOURCE(IDD_MAINDLG), NULL, MainDlgProc );
	ShowWindow(singleton->hDlg, SW_SHOWNORMAL);
	UpdateWindow(singleton->hDlg);

	return !!singleton->hDlg ;
}

INT_PTR CALLBACK MainDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	MainDlg* dlg = MainDlg::getSingleton();

	switch(uMsg) {
	case WM_INITDIALOG:
		dlg->onInitDialog();
		return TRUE;

	case WM_CLOSE:
		dlg->onClose();
		return TRUE;

	case WM_COMMAND:
		if( LOWORD(wParam) == IDB_RESUME && HIWORD(wParam) == BN_CLICKED ) {
			dlg->onResumeBtn();
		}
		else if( LOWORD(wParam) == IDB_PAUSE && HIWORD(wParam) == BN_CLICKED ) {
			dlg->onPauseBtn();
		}
		else if( LOWORD(wParam) == IDB_EXIT && HIWORD(wParam) == BN_CLICKED ) {
			dlg->onExitBtn();
		}
		else if( LOWORD(wParam) == IDB_ABOUT && HIWORD(wParam) == BN_CLICKED ) {
			dlg->onAboutBtn();
		}
		return TRUE;

	case WM_DESTROY:
		dlg->onDestroy();
		return TRUE;

	default:
		return FALSE;
	}
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static bool lbtn = false ;
	static int prevX ;
	static int prevY ;

	switch(uMsg) {
	case WM_INITDIALOG:
		{
			RECT rc;
			GetClientRect(hDlg, &rc) ;
			MoveWindow(hDlg, 300, 100, rc.right, rc.bottom, TRUE) ;

			PlaySound(L"krang.wav", NULL, SND_FILENAME|SND_ASYNC) ;
		}
		return TRUE;

	case WM_COMMAND:
		if( LOWORD(wParam) == IDC_CLOSEBTN && HIWORD(wParam) == BN_CLICKED ) {
			EndDialog(hDlg, 0) ;
		}
		return TRUE;

	case WM_LBUTTONDOWN:
		if(!lbtn)
		{
			lbtn = true ;
			RECT rc;
			GetWindowRect(hDlg, &rc) ;
			prevX = GET_X_LPARAM(lParam) + rc.left; 
			prevY = GET_Y_LPARAM(lParam) + rc.top;
		}
		return TRUE;

	case WM_MOUSEMOVE:
		if( lbtn && (wParam & MK_LBUTTON) ) {
			RECT rc ;
			GetWindowRect(hDlg, &rc) ;
			int x = GET_X_LPARAM(lParam) + rc.left; 
			int y = GET_Y_LPARAM(lParam) + rc.top;
			MoveWindow(hDlg, rc.left + (x-prevX), rc.top + (y-prevY), rc.right-rc.left, rc.bottom-rc.top, TRUE) ;
			prevX = x ;
			prevY = y ;
		}
		return TRUE ;

	case WM_LBUTTONUP:
		lbtn = false ;

		return TRUE ;

	default:
		return FALSE;
	}
}

void MainDlg::onClose() {
	DestroyWindow(hDlg) ;
}

void MainDlg::onDestroy() {
	PostQuitMessage(0) ;
}

void MainDlg::onAboutBtn() {
	DialogBox( (HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hDlg, AboutDlgProc ) ;
}

void MainDlg::onExitBtn() {
	PostMessage(hDlg, WM_CLOSE, 0, 0);
}

void MainDlg::onPauseBtn() {
	Simulation *psim = Simulation::getSingleton() ;
	psim->pause() ;
}

void MainDlg::onResumeBtn() {
	Simulation *psim = Simulation::getSingleton() ;
	psim->resume() ;
}

void MainDlg::onInitDialog() {
	SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETRANGE, 0, MAKELPARAM(0,100)) ;
}

void MainDlg::updateProgress() {
	const Simulation *psim = Simulation::getSingleton() ;
	SendDlgItemMessage(hDlg, IDC_PROGRESS, PBM_SETPOS, (int) psim->progress(), 0) ;
}

void MainDlg::updateStatus() {
	const Simulation *psim = Simulation::getSingleton() ;
	std::wstring status = L"Status: " + psim->status() ;
	SetDlgItemText(hDlg, IDC_STATUSLABEL, status.c_str()) ;
}

void MainDlg::log(const std::wstring &str) {
	SendDlgItemMessage(singleton->hDlg, IDC_OUT_EDIT, EM_SETSEL, -1, 0);
	SendDlgItemMessage(singleton->hDlg, IDC_OUT_EDIT, EM_REPLACESEL, 0, (LPARAM) str.c_str());
}
