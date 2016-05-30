#include "AnnWindow.h"
#include "Simulation.h"
#include "NeuralNetwork.h"

AnnWindow* AnnWindow::singleton = 0;

LRESULT CALLBACK AnnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) ;

void AnnWindow::create(HINSTANCE hInstance) {
	const wchar_t szClassName[] = L"AnnWindow" ;
	const wchar_t szWindowName[] = L"Neural Network Visualization";

	WNDCLASSEX wcex ;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.hCursor = LoadCursor(0, IDC_CROSS);
	wcex.hbrBackground = (HBRUSH) GetStockObject( BLACK_BRUSH );
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = AnnWndProc;
	wcex.lpszClassName = szClassName;
	wcex.lpszMenuName = 0 ;
	wcex.style = CS_VREDRAW | CS_HREDRAW;

	RegisterClassEx(&wcex) ;
	singleton = new AnnWindow() ;

	HWND hWnd = CreateWindowEx(0, szClassName, szWindowName, WS_OVERLAPPEDWINDOW|WS_HSCROLL, 0, 450, 1000, 480, 0, 0, hInstance, 0);
	ShowWindow(hWnd, SW_SHOWNORMAL);
}

void AnnWindow::onCreate(HWND hWnd) {
	this->hWnd = hWnd ;
}

void AnnWindow::onSize(unsigned w, unsigned h) {
	NeuralNetwork *pnn = Simulation::getSingleton()->getAnn() ;

	if(!!pnn) {
		pnn->resize(w,h/2);
	}
}

void AnnWindow::onPaint() {
	PAINTSTRUCT ps ;
	HDC hDC = BeginPaint(hWnd, &ps);

	NeuralNetwork *pnn = Simulation::getSingleton()->getAnn() ;

	SetTextColor(hDC, RGB(0xff,0xff,0xaf));
	SetBkMode(hDC, TRANSPARENT) ;

	if(!!pnn) {
		pnn->blt(hDC) ;


		RECT rc ;
		std::wstring ntext = pnn->getNeuronText() ;
		std::wstring ctext = pnn->getStatText() ;

		GetClientRect(hWnd, &rc) ;

		rc.top = rc.bottom / 2 ;

		FillRect(hDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

		rc.right /= 2 ;
		DrawText(hDC, ntext.c_str(), ntext.length(), &rc, DT_EXPANDTABS) ;

		rc.left = rc.right ;
		rc.right += rc.right ;
		DrawText(hDC, ctext.c_str(), ctext.length(), &rc, DT_EXPANDTABS) ;
	}

	EndPaint(hWnd, &ps);
}

void AnnWindow::update() {
	NeuralNetwork *pnn = Simulation::getSingleton()->getAnn() ;


	if(!!pnn) {
		pnn->redraw() ;
		InvalidateRect(hWnd, NULL, FALSE);
	}
}

void AnnWindow::displayLogWnd() {
	NeuralNetwork *pnn = Simulation::getSingleton()->getAnn() ;

	HWND hEdit = CreateWindowEx(WS_EX_TOPMOST, L"EDIT", L"Simulation Log", 
		ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_READONLY | WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, 0, 0, (HINSTANCE)GetModuleHandle(0), 0) ;

	std::wstring text = pnn->getLogText() ;
	SendMessage(hEdit, EM_SETSEL, 0, -1) ;
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM) text.c_str());

	ShowWindow(hEdit, SW_SHOWNORMAL);
}

LRESULT CALLBACK AnnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	AnnWindow *pann = AnnWindow::getSingleton() ;

	switch(uMsg) {
	case WM_CREATE:
		pann->onCreate(hWnd);
		break;

	case WM_SIZE:
		pann->onSize(LOWORD(lParam),HIWORD(lParam));
		break;

	case WM_PAINT:
		pann->onPaint();
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0l ;
}
