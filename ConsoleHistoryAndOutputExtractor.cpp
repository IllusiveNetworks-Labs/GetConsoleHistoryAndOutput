#include "Windows.h"
#include "ConsoleHistoryAndOutputExtractor.h"
#include <vector>
#include "StringUtil.h"
#include "UnicodeUtil.h"

using namespace std;
const wchar_t* ConsoleHistoryAndOutputExtractor::COMMAND_LINE_PROCESS_NAME = L"cmd.exe";

ConsoleHistoryAndOutputExtractor::ConsoleHistoryAndOutputExtractor(const string& log_file_path) : m_log_file(log_file_path) {
}

void ConsoleHistoryAndOutputExtractor::_logMessageAndThrow(const string& message) {
    m_log_file.write(message);
    throw runtime_error(message);
}

void ConsoleHistoryAndOutputExtractor::_logMessageWithLastErrorAndThrow(const string& message) {
    _logMessageAndThrow(message + ". Error - " + to_string(::GetLastError()));
}

void ConsoleHistoryAndOutputExtractor::_getConsoleFunctions(GetConsoleCommandHistoryLengthWPtr &get_console_command_history_length_func, GetConsoleCommandHistoryWPtr &get_console_command_history_func) {
    HMODULE dll_handle = GetModuleHandleW(L"kernel32.dll");
    if (NULL == dll_handle) {
        _logMessageWithLastErrorAndThrow("Failed to get handle of kernel32.dll");
    }

    get_console_command_history_length_func = (GetConsoleCommandHistoryLengthWPtr)GetProcAddress(dll_handle, "GetConsoleCommandHistoryLengthW");

    if (NULL == get_console_command_history_length_func) {
        _logMessageWithLastErrorAndThrow("Failed to get GetConsoleCommandHistoryLengthW address");
    }

    get_console_command_history_func = (GetConsoleCommandHistoryWPtr)GetProcAddress(dll_handle, "GetConsoleCommandHistoryW");
    if (NULL == get_console_command_history_func) {
        _logMessageWithLastErrorAndThrow("Failed to get GetConsoleCommandHistoryW address");
    }
}

void ConsoleHistoryAndOutputExtractor::_writeConsoleCommandsHistory() {
    GetConsoleCommandHistoryLengthWPtr get_console_command_history_length_func;
    GetConsoleCommandHistoryWPtr get_console_command_history_func;

    _getConsoleFunctions(get_console_command_history_length_func, get_console_command_history_func);

    DWORD historyBuffSize = get_console_command_history_length_func((LPWSTR)COMMAND_LINE_PROCESS_NAME);
    if (0 == historyBuffSize) {
		_logMessageWithLastErrorAndThrow("Console command history is of zero length");		
    }

    // historyBuffSize is returned in bytes
    // convert byte count to wchar_t count
    const size_t history_buffer_length = historyBuffSize / sizeof(wchar_t);
    vector<wchar_t> output_history_buffer(history_buffer_length);

    if (0 == get_console_command_history_func(output_history_buffer.data(), historyBuffSize, (LPWSTR)COMMAND_LINE_PROCESS_NAME)) {
		_logMessageWithLastErrorAndThrow("Failed to get console history");
    }

    // print the null delimited buffer of history string commands
	m_log_file.write("\n## Process' Command History ##");
    size_t current_location = 0; 
    unsigned int command_counter = 0;         
    while (current_location < history_buffer_length) {
        // point to the current command string in outputHistoryBuff
        wstring current_history_command(output_history_buffer.data() + current_location);
        m_log_file.write(string("H") + to_string(command_counter) + ": " + utf16ToUtf8(current_history_command));

        current_location += current_history_command.size() + 1;
        command_counter++;
    }
}

void ConsoleHistoryAndOutputExtractor::_writeConsoleOutputBuffer() {
	const SHORT MAX_BUFFER_SIZE = 4096;
	SHORT line_count = 0;
	SHORT starting_line = 0;
	

    HANDLE console_output_handle = ::GetStdHandle(STD_OUTPUT_HANDLE); 
    if (INVALID_HANDLE_VALUE == console_output_handle) {
        _logMessageWithLastErrorAndThrow("GetStdHandle failed");
    }

    CONSOLE_SCREEN_BUFFER_INFO console_screen_buffer_info;
    // Get the current screen buffer size and window position. 
    if (0 == GetConsoleScreenBufferInfo(console_output_handle, &console_screen_buffer_info)) {
        _logMessageWithLastErrorAndThrow("GetConsoleScreenBufferInfo Failed");
    }

	m_log_file.write("\n## Console Output Buffer ##");

	while (starting_line < console_screen_buffer_info.dwSize.Y) {
		line_count = min(MAX_BUFFER_SIZE / console_screen_buffer_info.dwSize.X, console_screen_buffer_info.dwSize.Y - starting_line);
		if (0 == _writeConsoleOutputBufferBySize(console_output_handle, starting_line, console_screen_buffer_info.dwSize.X, line_count)) {
			break;
		}
		starting_line += line_count;

	}

}

int ConsoleHistoryAndOutputExtractor::_writeConsoleOutputBufferBySize(HANDLE &console_output_handle, SHORT starting_line, SHORT line_length, SHORT line_count) {

	// The read coordinates of the upper-left cell of the screen buffer    
	COORD buffer_coordinates = { 0, 0 };
	// The size of the output screen buffer (charInfoBuffer)    
	COORD coordinated_buffer_size = { line_length, line_count };
	// The current screen buffer rectangle     
	SMALL_RECT current_screen_buffer_rectangle{ 0, starting_line, line_length, starting_line+line_count };
	// The output screen buffer    
	vector<CHAR_INFO> output_screen_buffer(line_length * line_count);

	BOOL succeeded = ReadConsoleOutput(
		console_output_handle,                 // An handle to the console screen buffer 
		output_screen_buffer.data(),           // A pointer to a destination buffer
		coordinated_buffer_size,               // The size of the charInfoBuffer parameter 
		buffer_coordinates,                    // The read coordinates of the upper-left cell 
		&current_screen_buffer_rectangle);     // The current screen buffer rectangle 

	if (!succeeded) {
		_logMessageWithLastErrorAndThrow("ReadConsoleOutput failed");
	}

	CHAR_INFO *current_line_location = NULL;	
	int printed_line_count = 0;
	for (int line_number = 0; line_number < line_count; line_number++) {
		// Current line's position at charInfoBuffer
		current_line_location = output_screen_buffer.data() + (line_number * line_length);

		// The current line string of the console buffer rectangle        
		wstring currLine;
		for (int j = 0; j < line_length; j++) {
			// Add current char to the current line
			currLine += (wchar_t)current_line_location[j].Char.UnicodeChar;
		}
		// Skip lines that are composed only from space chars
		const string utf8_line = utf16ToUtf8(currLine);
		const string trimmed_line = trimStr(utf8_line);
		if (!trimmed_line.empty()) {
			m_log_file.write(string("L") + to_string(starting_line + line_number) + ": " + utf8_line);
			printed_line_count++;
		}
	}
	return printed_line_count;
}
void ConsoleHistoryAndOutputExtractor::extract(int pid) {
    // We must detach current console before attaching to a console of another proces
    FreeConsole();
    if (0 == AttachConsole(pid)) {
        const string message(string("Failed attaching to console of process ") + to_string(pid)
            + ", error code : " + to_string(::GetLastError()));
        m_log_file.write(message);
        throw runtime_error(message);
    }

    m_log_file.write(string("### Display Output of PID: ") + to_string(pid) + "###");
    try {
        _writeConsoleCommandsHistory();
    }
    catch (...) {
        //on failure we move on
        //since we are detached now from the console, we do not write into it
    }
    try {
        _writeConsoleOutputBuffer();
    }
    catch (...) {
        //on failure we move on
        //since we are detached now from the console, we do not write into it
    }
    m_log_file.write("###        Output End         ###");
}


