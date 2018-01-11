#pragma once
#include <string>
#include <fstream>

class LogFile {
public:
    LogFile(const std::string& utf16_log_file_path);
    LogFile(const LogFile& other) = delete;

    void write(const std::wstring& utf16_line);
    void write(const std::string& utf8_line);

private:
    std::wstring m_log_file_path;
    std::ofstream m_log_file_stream;
    
};