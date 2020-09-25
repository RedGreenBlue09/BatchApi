#include <iostream>
#include <string>
#include <string_view>
#include <windows.h>

using uint = unsigned int;
using namespace std;

string toLower(string_view str) {
	string answer;
	for (uint count = 0; count < str.length(); ++count) {
		char at = str.at(count);
		if ((at >= 'A') && (at <= 'Z')) {
			answer += at + 32; // +32 is the distance between lowercase char and uppercase char in ascii.
		} else {
			answer += at;
		}
	}
	return answer;
}

bool isNumber(string_view num) {
	for (uint cc = 0; cc < num.length(); ++cc) {
		if ((num.at(cc) < '0') || (num.at(cc) > '9')) {
			return false;
		}
	}
	return true;
}

bool isHex(string_view num) {
	for (uint cc = 0; cc < num.length(); ++cc) {
		if (num.at(cc) < '0') {
			return false;
		} else if ((num.at(cc) > '9') && (num.at(cc) < 'a')) {
			return false;
		} else if (num.at(cc) > 'f') {
			return false;
		}
	}
	return true;
}

string WideToMultiByte(wstring wstr, uint cp) {
	uint size = WideCharToMultiByte(cp, 0, &wstr[0], -1, 0, 0, NULL, NULL);
	string str(size, 0);
	WideCharToMultiByte(cp, 0, &wstr[0], -1, &str[0], size, NULL, NULL);
	return str;
}
