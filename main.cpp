#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
#include "UnicodeUtil.h"
#include "ProcessUtil.h"
#include "ConsoleHistoryAndOutputExtractor.h"

using namespace std;

int run(int argc, wchar_t *argv[]) {
    if (argc != 3) {
        wcout << L"Usage: GetConsoleHistoryAndOutput.exe pid_of_console_process outputfile_path" << endl;
        return EXIT_FAILURE;
    }     
    unsigned int pid = wcstoul(argv[1], NULL, 0);
	wstring output_file_path(argv[2]);
	wstring process_name;
    if (!isRunning(pid, process_name)) {
        wcout << L"Process pid " << argv[1] << L" was not found" << endl;
        return EXIT_FAILURE;
    }
    else {
        wcout << L"Starting GetConsoleHistoryAndOutput on proccess pid: " << pid << L", name: " << process_name << endl;
    }

	wcout << L"Detaching console - further logs will be written to the output file: " << output_file_path << endl;
    ConsoleHistoryAndOutputExtractor console_history_and_output_extractor(utf16ToUtf8(output_file_path));
	console_history_and_output_extractor.extract(pid);
    return EXIT_SUCCESS;
}


int wmain(int argc, wchar_t *argv[]) {
    try {
        return run(argc, argv);
    }
    catch (const exception& e) {
		cerr << e.what() << endl;
    }
    catch (...) {
    }
    return EXIT_FAILURE;
}
