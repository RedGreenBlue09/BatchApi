#include <iostream>
#include <cassert>
#include <windows.h>

using namespace std;
using uint = unsigned int;
const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

// Basic.cpp
string WideToMultiByte(wstring wstr, uint cp);

// WinApi.cpp
COORD GetBufferSize() {
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (GetConsoleScreenBufferInfo(handle, &cbsi)) {
		return cbsi.dwSize;
	} else {
		cerr << "Failed to get buffer size.\r\n" << flush;
		return { 0, 0 };
	}
}

int ResizeBuffer(SHORT& x, SHORT& y) {
	if (!SetConsoleScreenBufferSize(handle, { x, y })) {
		cerr << "Failed to resize buffer.\r\n" << flush;
		return GetLastError();
	}
	return 0;
}

SMALL_RECT GetWindowSize() {
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (GetConsoleScreenBufferInfo(handle, &cbsi)) {
		return cbsi.srWindow;
	} else {
		cerr << "Failed to get window size.\r\n" << flush;
		return { 0, 0, 0, 0 };
	}
}

int ResizeWindow(SHORT& x, SHORT& y) {
	SMALL_RECT windowSize = { 0, 0, x - 1, y - 1 };
	if (!SetConsoleWindowInfo(handle, true, &windowSize)) {
		cerr << "Failed to resize window.\r\n" << flush;
		return GetLastError();
	}
	return 0;
}

// Max possible size compares to the screen.
COORD GetLargestWindowSize() {
	COORD lsize = GetLargestConsoleWindowSize(handle);
	if (GetLastError() == 0) {
		return lsize;
	} else {
		cerr << "Failed to get largest posible window size.\r\n" << flush;
		return { 0, 0 };
	}
}

// Max size compares to the console buffer.
/*
COORD GetLargestWindowSize2() {
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (GetConsoleScreenBufferInfo(handle, &cbsi)) {
		return cbsi.dwMaximumWindowSize;
	}
	else {
		cerr << "Failed to get window size.\r\n" << flush;
		return { 0, 0 };
	}
}
*/

COORD GetCursorPosition() {
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (GetConsoleScreenBufferInfo(handle, &cbsi)) {
		return cbsi.dwCursorPosition;
	} else {
		cerr << "Failed to get cursor position.\r\n" << flush;
		return { 0, 0 };
	}
}

int SetCursorPosition(SHORT& x, SHORT& y) {
	COORD cursorPos = { x, y };
	if (!SetConsoleCursorPosition(handle, cursorPos)) {
		cerr << "Failed to change cursor position.\r\n" << flush;
		return GetLastError();
	}
	return 0;
}

int SetTextAttribute(uint &attr) {
	if (!SetConsoleTextAttribute(handle, attr)) {
		cerr << "Failed to change text attributes.\r\n" << flush;
		return GetLastError();
	}
	return 0;
}

