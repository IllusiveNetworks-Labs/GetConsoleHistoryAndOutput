#include "LogFile.h"
#include "UnicodeUtil.h"

using namespace std;

LogFile::LogFile(const string& log_file_path) :
    m_log_file_path(utf8ToUtf16(log_file_path)), m_log_file_stream(m_log_file_path.c_str(), ios::binary | ios::app | ios::out) {

}

void LogFile::write(const wstring& utf16_line) {
    write(utf16ToUtf8(utf16_line));
}

void LogFile::write(const string& utf8_line) {
    m_log_file_stream << utf8_line << endl;
}
