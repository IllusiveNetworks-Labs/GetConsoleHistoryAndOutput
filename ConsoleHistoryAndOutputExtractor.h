#pragma once
#include "Windows.h"
#include "LogFile.h"


// GetConsoleCommandHistoryLengthW - Retrieves the buffer size of the command line history
// Input - Procces name (i.e "cmd.exe")
// Output - Buffer size in bytes (not wchar_t), zero in case of failure
using GetConsoleCommandHistoryLengthWPtr = DWORD(WINAPI *)(IN LPWSTR sExeName);

// GetConsoleCommandHistoryW - Retrieves the command line history buffer
// Input - Output buffer size in bytes and process name
// Output - Null delimited buffer of history string commands
using GetConsoleCommandHistoryWPtr = DWORD(WINAPI *) (OUT LPWSTR sCommands, IN DWORD nBufferLength, IN LPWSTR sExeName);

class ConsoleHistoryAndOutputExtractor {
public:
    ConsoleHistoryAndOutputExtractor(const std::string& log_file_path);
    ConsoleHistoryAndOutputExtractor(const ConsoleHistoryAndOutputExtractor& other) = delete;	

	void extract(int pid);

private:
	static const wchar_t* COMMAND_LINE_PROCESS_NAME;

    void _writeConsoleCommandsHistory(); // Print commands history of cmd.exe only
    void _writeConsoleOutputBuffer();
	int _writeConsoleOutputBufferBySize(HANDLE &console_output_handle, SHORT start_line, SHORT line_length, SHORT line_count);
    void _getConsoleFunctions(GetConsoleCommandHistoryLengthWPtr &get_console_command_history_length_func,
        GetConsoleCommandHistoryWPtr &get_console_command_history_func); // Retrive handles for GetConsoleCommandHistoryLengthW and GetConsoleCommandHistoryW

    void _logMessageAndThrow(const std::string& message);
    void _logMessageWithLastErrorAndThrow(const std::string& message);
    LogFile m_log_file;	
};