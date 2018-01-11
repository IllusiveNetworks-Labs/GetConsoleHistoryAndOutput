#pragma once
#include <string>

std::string utf16ToUtf8(const std::wstring& utf16_string);
std::wstring utf8ToUtf16(const std::string& utf8_string);