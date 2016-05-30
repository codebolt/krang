#pragma once

#include "WinInclude.h"
#include <string>

class MainDlg {
	static MainDlg* singleton ;
	HWND hDlg ;

	MainDlg() { } ;

public:
	static bool create(HINSTANCE hInstance) ;
	static MainDlg* getSingleton() { return singleton; }
	static void log(const std::wstring& str);

	void updateProgress() ;	// update progress bar
	void updateStatus() ;	// update status label

	void onInitDialog();
	void onClose();
	void onResumeBtn();
	void onPauseBtn();
	void onAboutBtn();
	void onExitBtn();
	void onDestroy();
} ;

