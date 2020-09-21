#include <iostream>
#include <string>
#include <string_view>
#include <windows.h>

using namespace std;
using uint = unsigned int;
constexpr int buildNumber = 6;

// Basic.cpp
string toLower(string_view str);

// Command.cpp
namespace Command {
	void   help       (string_view programName);
	int    getsize    (char** argv);
	int    setsize    (int& argc, char** argv);
	int    getmaxsize (char** argv);
	int    getcursor  (char** argv);
	int    setcursor  (char** argv);
	int    setattr    (char** argv);
	int    gettitle   ();
	int    settitle   (char** argv);
}

// Main.cpp
int main(int argc, char** argv) {
	if (argc == 1) {
		Command::help(argv[0]);
		return 0;
	} else if ((argv[1] == "help") || (toLower(argv[1]) == "?")) {
		if (argc == 2) {
			Command::help(argv[0]);
			return 0;
		} else {
			cerr << "Invalid argument(s) entered.\r\n" << flush;
			Command::help(argv[0]);
			return 1;
		}
	} else if (toLower(argv[1]) == "ver") {
		if (argc == 2) {
			cout << "Build " << buildNumber << "\r\n" << flush;
			return 0;
		} else {
			cerr << "Invalid argument(s) entered.\r\n" << flush;
			Command::help(argv[0]);
			return 1;
		}
	} else if (toLower(argv[1]) == "getsize") {
		if (argc == 3) {
			return Command::getsize(argv);
		} else {
			cerr << "Invalid argument(s) entered.\r\n" << flush;
			Command::help(argv[0]);
			return 1;
		}
	} else if (toLower(argv[1]) == "setsize") {
		return Command::setsize(argc, argv);
	} else if (toLower(argv[1]) == "getmaxsize") {
		if (argc == 3) {
			return Command::getmaxsize(argv);
		} else {
			cerr << "Invalid argument(s) entered.\r\n" << flush;
			Command::help(argv[0]);
			return 1;
		}
	} else if (toLower(argv[1]) == "getcursor") {
		if (argc == 3) {
			return Command::getcursor(argv);
		} else {
			cerr << "Invalid argument(s) entered.\r\n" << flush;
			Command::help(argv[0]);
			return 1;
		}
	} else if (toLower(argv[1]) == "setcursor") {
		if (argc == 4) {
			return Command::setcursor(argv);
		} else {
			cerr << "Invalid argument(s) entered.\r\n" << flush;
			Command::help(argv[0]);
			return 1;
		}
	} else if (toLower(argv[1]) == "setattr") {
		if (argc == 3) {
			return Command::setattr(argv);
		} else {
			cerr << "Invalid argument(s) entered.\r\n" << flush;
			Command::help(argv[0]);
			return 1;
		}
	} else if (toLower(argv[1]) == "gettitle") {
		if (argc == 2) {
			return Command::gettitle();
			return 0;
		} else {
			cerr << "Invalid argument(s) entered.\r\n" << flush;
			Command::help(argv[0]);
			return 1;
		}
	} else if (toLower(argv[1]) == "settitle") {
		return Command::settitle(argv);
	} else {
		cerr << "Invalid argument(s) entered.\r\n" << flush;
		Command::help(argv[0]);
		return 1;
	}

	return 0;
}
