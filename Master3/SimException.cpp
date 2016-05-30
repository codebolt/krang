#include "SimException.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void SimException::log() {
	std::wstring display = this->file + L"(" + x2s(this->line) + L"):\r\n\r\n" + this->str ;
	MessageBox(0, display.c_str(), L"Exception", MB_ICONEXCLAMATION);
}
