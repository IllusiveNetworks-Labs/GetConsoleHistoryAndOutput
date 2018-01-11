#include "Windows.h"
#include "tlhelp32.h"
#include "ProcessUtil.h"
#include "UnicodeUtil.h"

using namespace std;

bool isRunning(unsigned int pid, wstring& process_name) {
    HANDLE pss = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32W pe = { 0 };
    pe.dwSize = sizeof(pe);

    if (Process32First(pss, &pe)) {
        do {
            if (pe.th32ProcessID == pid) {
                process_name = pe.szExeFile;
				CloseHandle(pss);
                return true;				
            }
        } while (Process32Next(pss, &pe));
    }
    CloseHandle(pss);
    return false;
}