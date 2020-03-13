#include "charfix.h"
#include <string>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <iomanip>
#include <sstream>
#include <windows.h>
#include <winternl.h>
#include <tlhelp32.h>
#include "utils.h"
#include "legacy.h"
#include "console.h"

std::string Utils::UnixDate(int date) 
{
    std::uint32_t time_date_stamp = date;
    time_t temp = time_date_stamp;
    std::tm* t = gmtime(&temp);
    std::stringstream ss;
    ss << std::put_time(t, "%Y-%m-%d %I:%M:%S %p");
    std::string output = ss.str();
    return output;
}

int Utils::FindProcess(const wchar_t* proc)
{
	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	auto pe = PROCESSENTRY32{ sizeof(PROCESSENTRY32) };

	if (Process32First(snapshot, &pe)) {
		do {
			if (wcscmp(proc, pe.szExeFile) == 0) {
				CloseHandle(snapshot);
				return pe.th32ProcessID;
			}
		} while (Process32Next(snapshot, &pe));
	}
	CloseHandle(snapshot);
	return 0;
}

uintptr_t Utils::GetBase(int pid, const char* modulename) 
{
    ModuleInfo mi = GetProcessModules(pid);
    for (int i = 0; i < MODULE_MAX; i++) 
    {
        ModuleDll mod = mi.list[i];
        if (!mod.name) continue;
        
        if (strstr(mod.name, modulename)) 
        {         
            return mod.baseaddress;
        }
    }
    return 0;
}

static DWORD LastFrameTime = 0;
void Utils::LimitFPS(int targetfps) 
{
    DWORD currentTime = timeGetTime();
    if ( (currentTime - LastFrameTime) < (1000 / targetfps))
    {
        Sleep(currentTime - LastFrameTime);
    }
    LastFrameTime = currentTime;   
}