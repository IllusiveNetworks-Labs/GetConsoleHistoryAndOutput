#include "Windows.h"
#include "UnicodeUtil.h"
#include <stdexcept>
#include <vector>

using namespace std;

string utf16ToUtf8(const wstring& utf16_string) {
	const int len_of_utf8_string = ::WideCharToMultiByte(CP_UTF8, 0, utf16_string.c_str(), -1,
		NULL, 0, NULL, NULL);
	if (len_of_utf8_string == 0) {
		throw runtime_error("Failed converting utf-16 to utf-8");
	}
	vector<char> buffer(len_of_utf8_string, '\0');
	if (::WideCharToMultiByte(CP_UTF8, 0, utf16_string.c_str(), -1,
		buffer.data(), buffer.size(), NULL, NULL) != len_of_utf8_string) {
		throw runtime_error("Failed converting utf-16 to utf-8");
	}
	return string(buffer.data());
}

wstring utf8ToUtf16(const string& utf8_string) {
	const int len_of_utf16_string_in_characters = ::MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1,
		NULL, 0);
	if (len_of_utf16_string_in_characters == 0) {
		throw runtime_error("Failed converting utf-8 to utf-16");
	}
	const int len_of_utf16_string_in_bytes = sizeof(wchar_t) * len_of_utf16_string_in_characters;
	vector<char> buffer(len_of_utf16_string_in_bytes, '\0');
	if (::MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1,
		reinterpret_cast<wchar_t*>(buffer.data()), buffer.size()) != len_of_utf16_string_in_characters) {
		throw runtime_error("Failed converting utf-8 to utf-16");
	}
	return wstring(reinterpret_cast<wchar_t*>(buffer.data()));
}