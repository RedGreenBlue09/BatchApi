#include <iostream>
#include <string>
#include <string_view>
#include <cctype>
#include <windows.h>

using namespace std;
using uint = unsigned int;
using ushort = unsigned short;
constexpr int buildNumber = 2;

// Basic.cpp
string  toLower   (string_view str);
bool    isNumber  (string_view num);
bool    isHex     (string_view num);

// WinApi.cpp
int     GetBufferSize         (OUT COORD& bufferSize);
int     ResizeBuffer          (SHORT& x, SHORT& y);
int     GetWindowSize         (OUT SMALL_RECT& windowSize);
int     ResizeWindow          (SHORT& x, SHORT& y);
int     GetLargestWindowSize  (OUT COORD& lSize);
int     GetCursorPosition     (OUT COORD& cursorPos);
int     SetCursorPosition     (SHORT& x, SHORT& y);
int     SetTextAttribute      (uint& attr);
int     SetSpecTextAttribute  (ushort* attr, uint& length, COORD coord);
int     CoordToLength         (COORD& a, COORD& b, OUT uint& totalLen);

namespace Command {
	void help(string_view programName) {
		string help;
		help += "\n"
				"Customize a Windows Command Processor window.\r\n"
				"\r\n  ";
		help += programName; help += " Command [Options...]\r\n"
				"\r\n"
				"Commands:\r\n"
				"  Name [Args]             Description\r\n"
				"  --------------          --------------\r\n"
				"  help                    Show this help message.\r\n"
				"  ver                     Show build number.\r\n"
				"  getsize BufferX|BufferY|WindowX|WindowY\r\n"
				"                          Get current buffer size or window size.\r\n"
				"  setsize BufferX BufferY WindowX WindowY [/f]\r\n"
				"                          Resize the console buffer and window.\r\n"
				"  getmaxsize              Get maximum possible window size\r\n"
				"                          that fit your screen.\r\n"
				"  getcursor X|Y           Get current cursor position.\r\n"
				"  setcursor X Y           Set cursor position.\r\n"
				"  setattr Attr [/c]       Set text attributes.\r\n"
				"  gettitle                Get current title.\r\n"
				"  settitle Title          Set title. (\"title\" alternative)\r\n"
				"\r\n"
				"Options:\r\n"
				"  Name       Command      Description\r\n"
				"  -------    ----------   --------------\r\n"
				"  /fsz       setsize      Resize the window to max size when the requested size\r\n"
				"                          is larger than possible size.\r\n"
				"  /c x1 y1 x2 y2"
				"             setattr      Specific the coordinates of characters\r\n"
				"                          to apply attributes.\r\n"
				"  /?  /help  <All>        Show help message of a command. (Comming Soon!)\r\n"
				;
		cout << help << flush;
	}

	int getsize(char** argv) {
		COORD bufferSize = { 0, 0 };
		SMALL_RECT windowSize = { 0, 0, 0, 0 };
		if (int errorCode = GetBufferSize(OUT bufferSize) != 0) {
			return errorCode;
		}
		if (int errorCode = GetWindowSize(OUT windowSize) != 0) {
			return errorCode;
		}
		if (toLower(argv[2]) == "bufferx") {
			cout << bufferSize.X << "\r\n" << flush;
			return 0;
		} else if (toLower(argv[2]) == "buffery") {
			cout << bufferSize.Y << "\r\n" << flush;
			return 0;
		} else if (toLower(argv[2]) == "windowx") {
			cout << windowSize.Right + 1 - windowSize.Left << "\r\n" << flush;
			return 0;
		} else if (toLower(argv[2]) == "windowy") {
			cout << windowSize.Bottom + 1 - windowSize.Top << "\r\n" << flush;
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
		bool option_fsz = false;
		if (argc > 6) {
			// This loop is to avoid rewriting if another option was added.
			for (int count = 6; count < argc; ++count) {
				if (toLower(argv[count]) == "/fsz") {
					option_exist = true;
					option_fsz = true;
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
		SMALL_RECT windowSize = { 0, 0, 0, 0 };
		if (int errorCode = GetWindowSize(OUT windowSize) != 0) {
			return errorCode;
		}
		SHORT currentWinX = windowSize.Right;
		SHORT currentWinY = windowSize.Bottom;
		if (currentWinX == 0 || currentWinY == 0) {
			return 3;
		}
		// Narrow convert long to unsigned int.
		SHORT bufX = (SHORT)stoul(argv[2]);
		SHORT bufY = (SHORT)stoul(argv[3]);
		SHORT winX = (SHORT)stoul(argv[4]);
		SHORT winY = (SHORT)stoul(argv[5]);
		if (option_fsz == true) {
			COORD maxWinSize = { 0, 0 };
			if (int errorCode = GetLargestWindowSize(OUT maxWinSize) != 0) {
				return errorCode;
			}
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
		COORD maxSize;
		if (int errorCode = GetLargestWindowSize(OUT maxSize) != 0) {
			return errorCode;
		}
		if (toLower(argv[2]) == "x") {
			cout << maxSize.X << "\r\n" << flush;
			return 0;
		} else if (toLower(argv[2]) == "y") {
			cout << maxSize.Y << "\r\n" << flush;
			return 0;
		} else {
			cerr << "Invalid argument(s) entered.\r\n" << flush;
			Command::help(argv[0]);
			return 1;
		}
		return 0;
	}

	int getcursor(char** argv) {
		COORD cursorPos = { 0, 0 };
		if (int errorCode = GetCursorPosition(OUT cursorPos) != 0) {
			return errorCode;
		}
		if (toLower(argv[2]) == "x") {
			cout << cursorPos.X << "\r\n" << flush;
			return 0;
		} else if (toLower(argv[2]) == "y") {
			cout << cursorPos.Y << "\r\n" << flush;
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

	int setattr(int& argc, char** argv) {
		bool option_exist = false;
		bool option_c = false;
		if (argc > 3) {
			// This loop is to avoid rewriting if another option was added.
			for (int count = 3; count < argc; ++count) {
				if (toLower(argv[count]) == "/c") {
					if (argc < 7) {
						cerr << "Invalid option(s) entered.\r\n" << flush;
						help(argv[0]);
						return 1;
					}
					option_exist = true;
					option_c = true;
				}
			}
			if (option_exist == false) {
				cerr << "Invalid option(s) entered.\r\n" << flush;
				help(argv[0]);
				return 1;
			}
		}
		if (isHex(toLower(argv[2])) == true) {
			uint attr = stoul(argv[2], nullptr, 16);
			if (option_c == true) {
				if (
					isNumber(argv[4]) == false ||
					isNumber(argv[5]) == false ||
					isNumber(argv[6]) == false ||
					isNumber(argv[7]) == false
					) {
					cerr << "Invalid number entered.\r\n"
							"All values must be natural numbers smaller than 32767.\r\n" << flush;
					return 1;
				} else if (
					// CMD's buffer size limit is 32766 cols and 32766 lines.
					// Use strtoul() to prevent out of range exception.
					strtoul(argv[4], nullptr, 10) > 32766 ||
					strtoul(argv[5], nullptr, 10) > 32766 ||
					strtoul(argv[6], nullptr, 10) > 32766 ||
					strtoul(argv[7], nullptr, 10) > 32766
					) {
					cerr << "Too large number entered.\r\n"
							"All values must be natural numbers smaller than 32767.\r\n" << flush;
					return 1;
				}
				SHORT x1 = (SHORT)stoul(argv[4], nullptr, 10);
				SHORT y1 = (SHORT)stoul(argv[5], nullptr, 10);
				SHORT x2 = (SHORT)stoul(argv[6], nullptr, 10);
				SHORT y2 = (SHORT)stoul(argv[7], nullptr, 10);
				COORD coordA = { x1, y1 };
				COORD coordB = { x2, y2 };
				uint length = 0;
				if (int errorCode = CoordToLength(coordA, coordB, OUT length) != 0) {
					return errorCode;
				}
				ushort* attrs = new ushort[length]{};
				for (uint count = 0; count < length; ++count) {
					attrs[count] = (ushort)attr;
				}
				SetSpecTextAttribute(attrs, length, coordA);
				delete[] attrs;
				return 0;
			} else {
				if (int errorCode = SetTextAttribute(attr) != 0) {
					return errorCode;
				}
				return 0;
			}
		} else {
			cerr << "Invalid hex number entered.\r\n" << flush;
			return 1;
		}
		return 0;
	}

	int gettitle() {
		char title[MAX_PATH];
		if (!GetConsoleTitleA(title, MAX_PATH)) {
			cerr << "Failed to get console title.\r\n";
			return GetLastError();
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
		while (*cmdLine == (uint)space[0]) {       // *cmdLine is first character of cmdLine.
			++cmdLine;
		}
		SetConsoleTitleW(cmdLine);
		return 0;
	}
}
