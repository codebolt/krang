#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class NeuralNetwork ;

class AnnWindow {
	static AnnWindow *singleton ;

	HWND hWnd ;
	AnnWindow() { }

public :
	static void create(HINSTANCE hInstance) ;
	static AnnWindow* getSingleton() { return singleton; }

	void onCreate(HWND hWnd) ;
	void onPaint() ;
	void onSize(unsigned w, unsigned h) ;
	void update() ;
	void displayLogWnd() ;
} ;