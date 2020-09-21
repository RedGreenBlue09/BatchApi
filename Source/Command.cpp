#include <iostream>
#include <string>
#include <string_view>
#include <cctype>
#include <windows.h>

using namespace std;
using uint = unsigned int;
constexpr int buildNumber = 1;

// Basic.cpp
string     toLower         (string_view str);
bool       isNumber        (string_view num);
bool       isHex           (string_view num);

// WinApi.cpp
COORD      GetBufferSize        ();
int        ResizeBuffer         (SHORT& x, SHORT& y);
SMALL_RECT GetWindowSize        ();
int        ResizeWindow         (SHORT& x, SHORT& y);
COORD      GetLargestWindowSize ();
COORD      GetCursorPosition    ();
int        SetCursorPosition    (SHORT& x, SHORT& y);
int        SetTextAttribute     (uint& attr);
string     GetTitle             ();

namespace Command {
	void help(string_view programName) {
		string help;
		help += "Customize a Windows Command Processor window.\r\n"
			"\r\n  ";
		help += programName; help += " Command [Options...]\r\n"
			"\r\n"
			"Commands:\r\n"
			"  help               Show this help message.\r\n"
			"  ver                Show build number.\r\n"
			"  getsize BufferX|BufferY|WindowX|WindowY\r\n"
			"                     Get current buffer size or window size.\r\n"
			"  setsize BufferX BufferY WindowX WindowY\r\n"
			"                     Resize the console buffer and window.\r\n"
			"  getmaxsize         Get maximum possible window size\r\n"
			"                     that fit your screen.\r\n"
			"  getcursor X|Y      Get current cursor position.\r\n"
			"  setcursor X Y      Set cursor position.\r\n"
			"  setattr Attr       Set text attributes.\r\n"
			"  gettitle           Get current title.\r\n"
			"  settitle Title     Set title. (\"title\" alternative)\r\n"
			"\r\n"
			"Options:\r\n"
			"  /fsz       Resize the window to max size\r\n"
			"             when the requested size is larger than possible size.\r\n"
			"  /?  /help  Show help message of a command. (Comming Soon!)\r\n"
			;
		cout << help << flush;
	}

	int getsize(char** argv) {
		COORD bufferSize = GetBufferSize();
		if (int errorCode = GetLastError() != 0) {
			cerr << "Failed to get console buffer size.";
			return errorCode;
		}
		SMALL_RECT windowSize = GetWindowSize();
		if (int errorCode = GetLastError() != 0) {
			cerr << "Failed to get window size.";
			return errorCode;
		}
		if (toLower(argv[2]) == "bufferx") {
			cout << bufferSize.X << "\r\n" << flush;
			return 0;
		} else if (toLower(argv[2]) == "buffery") {
			cout << bufferSize.Y << "\r\n" << flush;
			return 0;
		} else if (toLower(argv[2]) == "windowx") {
			cout << windowSize.Right + 1 - GetWindowSize().Left << "\r\n" << flush;
			return 0;
		} else if (toLower(argv[2]) == "windowy") {
			cout << windowSize.Bottom + 1 - GetWindowSize().Top << "\r\n" << flush;
			return 0;
		} else {
			cerr << "Invalid argument(s) entered.\r\n" << flush;
			help(argv[0]);
			return 1;
		}
		return 0;
	}

	int setsize(int& argc, char** argv) {
		bool option_exist = false;
		bool option_f = false;
		if (argc > 6) {
			// This loop is to avoid rewriting if another option was added
			for (int count = 6; count < argc; ++count) {
				if (toLower(argv[count]) == "/fsz") {
					option_exist = true;
					option_f = true;
				}
			}
			if (option_exist == false) {
				cerr << "Invalid option(s) entered.\r\n" << flush;
				help(argv[0]);
				return 1;
			}
		}
		if (
			isNumber(argv[2]) == false ||
			isNumber(argv[3]) == false ||
			isNumber(argv[4]) == false ||
			isNumber(argv[5]) == false
			) {
			cerr << "Invalid number entered.\r\n"
				"All values must be natural numbers smaller than 32767.\r\n" << flush;
			return 1;
		} else if (
			// CMD's buffer size limit is 32766 cols and 32766 lines.
			// Use strtoul() to prevent out of range exception.
			strtoul(argv[2], nullptr, 10) > 32766 ||
			strtoul(argv[3], nullptr, 10) > 32766 ||
			strtoul(argv[4], nullptr, 10) > 32766 ||
			strtoul(argv[5], nullptr, 10) > 32766
			) {
			cerr << "Too large number entered.\r\n"
				"All values must be natural numbers smaller than 32767.\r\n" << flush;
			return 1;
		}
		SHORT currentWinX = (SHORT)GetWindowSize().Right;
		SHORT currentWinY = (SHORT)GetWindowSize().Bottom;
		if (currentWinX == 0 || currentWinY == 0) {
			return 3;
		}
		// Narrow convert long to unsigned int.
		SHORT bufX = (SHORT)stoul(argv[2]);
		SHORT bufY = (SHORT)stoul(argv[3]);
		SHORT winX = (SHORT)stoul(argv[4]);
		SHORT winY = (SHORT)stoul(argv[5]);
		if (option_f == true) {
			COORD maxWinSize = GetLargestWindowSize();
			if (maxWinSize.X < winX) {
				winX = maxWinSize.X;
			}
			if (maxWinSize.Y < winY) {
				winY = maxWinSize.Y;
			}
		}
		if (winX > bufX) {
			cerr << "Window width cannot be larger than buffer width.\r\n" << flush;
			return 2;
		} else if (winY > bufY) {
			cerr << "Window heigth cannot be larger than buffer length.\r\n" << flush;
			return 2;
		} else if (currentWinX > bufX || currentWinY > bufY) {
			// This is a hard part to understand.
			// In case wanted buffer size is smaller than current window size,
			// we must resize the window to the wanted size first, then resize the buffer.
			// Remember buffer size always equal or larger than window size.
			if (int errorCode = ResizeWindow(winX, winY) != 0) {
				return errorCode;
			} else if (int errorCode = ResizeBuffer(bufX, bufY) != 0) {
				return errorCode;
			}
		} else {
			// This is also a hard part to understand.
			// In case current buffer size is smaller than wanted window size:
			// We compared this to the current window size because window size always equal or smaller than buffer size.
			// So we must resize the buffer to the wanted size first, then resize the window.
			if (int errorCode = ResizeBuffer(bufX, bufY) != 0) {
				return errorCode;
			} else if (int errorCode = ResizeWindow(winX, winY) != 0) {
				return errorCode;
			}
		}
		return 0;
	}

	int getmaxsize(char** argv) {
		if (toLower(argv[2]) == "x") {
			cout << GetLargestWindowSize().X << "\r\n" << flush;
			return 0;
		} else if (toLower(argv[2]) == "y") {
			cout << GetLargestWindowSize().Y << "\r\n" << flush;
			return 0;
		} else {
			cerr << "Invalid argument(s) entered.\r\n" << flush;
			Command::help(argv[0]);
			return 1;
		}
		return 0;
	}

	int getcursor(char** argv) {
		if (toLower(argv[2]) == "x") {
			cout << GetCursorPosition().X << "\r\n" << flush;
			return 0;
		} else if (toLower(argv[2]) == "y") {
			cout << GetCursorPosition().Y << "\r\n" << flush;
			return 0;
		} else {
			cerr << "Invalid argument(s) entered.\r\n" << flush;
			Command::help(argv[0]);
			return 1;
		}
		return 0;
	}

	int setcursor(char** argv) {
		if (
			isNumber(argv[2]) == false ||
			isNumber(argv[3]) == false
			) {
			cerr << "Invalid number entered.\r\n"
				"All values must be natural numbers smaller than 32767.\r\n" << flush;
			return 1;
		} else if (
			// CMD's buffer size limit is 32766 cols and 32766 lines.
			// Use strtoul() to prevent out of range exception.
			strtoul(argv[2], nullptr, 10) > 32766 ||
			strtoul(argv[3], nullptr, 10) > 32766
			) {
			cerr << "Too large number entered.\r\n"
				"All values must be natural numbers smaller than 32767.\r\n" << flush;
			return 1;
		}
		SHORT cursorX = (SHORT)stoul(argv[2]);
		SHORT cursorY = (SHORT)stoul(argv[3]);
		if (int errorCode = SetCursorPosition(cursorX, cursorY) != 0) {
			return errorCode;
		}
		return 0;
	}

	int setattr(char** argv) {
		if (isHex(toLower(argv[2])) == true) {
			uint attr = stoul(argv[2], nullptr, 16);
			if (int errorCode = SetTextAttribute(attr) == 0) {
				cerr << "Failed to change text attributes.\r\n" << flush;
				return -1;
			}
		} else {
			cerr << "Invalid hex number entered.\r\n" << flush;
			return 1;
		}
		return 0;
	}

	int gettitle() {
		string title = GetTitle();
		if (int errorCode = GetLastError() != 0) {
			return errorCode;
		} else {
			cout << title << "\r\n" << flush;
			return 0;
		}
		return 0;
	}

	int settitle(char** argv) {
		LPWSTR cmdLine = GetCommandLineW();
		for (uint count = 0; count < (strlen(argv[0]) + strlen(argv[1]) + 2); ++count) {
			++cmdLine;
		}
		wchar_t space[2] = L" ";
		while (*cmdLine == (uint)space[0]) {    // *cmdLine is first character of cmdLine.
			++cmdLine;
		}
		SetConsoleTitleW(cmdLine);
		return 0;
	}
}
