#include <iostream>
#include <cassert>
#include <windows.h>

using namespace std;
using uint = unsigned int;
using ushort = unsigned short;
const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

// Basic.cpp
string WideToMultiByte(wstring wstr, uint cp);

// WinApi.cpp
int GetBufferSize(OUT COORD& bufferSize) {
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (!GetConsoleScreenBufferInfo(handle, &cbsi)) {
		cerr << "Failed to get console buffer size.\r\n" << flush;
		return 3;
	}
	bufferSize = cbsi.dwSize;
	return 0;
}

int ResizeBuffer(SHORT& x, SHORT& y) {
	if (!SetConsoleScreenBufferSize(handle, { x, y })) {
		cerr << "Failed to resize buffer.\r\n" << flush;
		return GetLastError();
	}
	return 0;
}

int GetWindowSize(OUT SMALL_RECT& windowSize) {
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (!GetConsoleScreenBufferInfo(handle, &cbsi)) {
		cerr << "Failed to get window size.\r\n" << flush;
		return 3;
	}
	windowSize = cbsi.srWindow;
	return 0;
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
int GetLargestWindowSize(OUT COORD& lSize) {
	COORD lsize = GetLargestConsoleWindowSize(handle);
	if (int errorCode = GetLastError() != 0) {
		cerr << "Failed to get largest posible window size.\r\n" << flush;
		return errorCode;
	}
	lSize = lsize;
	return 0;
}

int GetCursorPosition(OUT COORD& cursorPos) {
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (!GetConsoleScreenBufferInfo(handle, &cbsi)) {
		cerr << "Failed to get cursor position.\r\n" << flush;
		return GetLastError();
	}
	cursorPos = cbsi.dwCursorPosition;
	return 0;
}

int SetCursorPosition(SHORT& x, SHORT& y) {
	COORD cursorPos = { x, y };
	if (!SetConsoleCursorPosition(handle, cursorPos)) {
		cerr << "Failed to change cursor position.\r\n" << flush;
		return GetLastError();
	}
	return 0;
}

int SetTextAttribute(uint& attr) {
	if (!SetConsoleTextAttribute(handle, attr)) {
		cerr << "Failed to change text attributes.\r\n" << flush;
		return GetLastError();
	}
	return 0;
}

int SetSpecTextAttribute(ushort* attr, uint& length, COORD coord) {
	DWORD written = 0;
	LPDWORD lpWritten = &written;
	if (!WriteConsoleOutputAttribute(handle, attr, length, coord, lpWritten)) {
		cerr << "Failed to change text attributes.\r\n" << flush;
		return GetLastError();
	}
	return 0;
}

int CoordToLength(COORD& a, COORD& b, OUT uint& totalLen) {
	if (a.Y > b.Y) {
		cerr << "Invalid coordinates passed.\r\n";
		return 2;
	} else if ((a.Y == b.Y) && (a.X > b.X)) {
		cerr << "Invalid coordinates passed.\r\n";
		return 2;
	}
	COORD bufferSize = { 0, 0 };
	if (int errorCode = GetBufferSize(OUT bufferSize) != 0) {
		return errorCode;
	}
	int aLineLen = bufferSize.X - a.X;
	int bLineLen = bufferSize.X - (bufferSize.X - b.X);
	int mLineLen = (b.Y - a.Y - 1) * bufferSize.X;
	totalLen = aLineLen + bLineLen + mLineLen + 1;
	return 0;
}
