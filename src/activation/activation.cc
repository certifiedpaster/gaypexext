#include "SlimMem.h"
#include "activation.h"
#include "../console.h"

SlimUtils::SlimMem mem;
#define LAUNCHER_PROCESS L"llauncher.exe"
#define LAUNCHER_OFFSET 0x58a10

bool Activation::Activate() 
{
    DWORD pid;
    
    Console::WriteLog("Activating...");
    if (!SlimUtils::SlimMem::GetPID(LAUNCHER_PROCESS, &pid)) 
    {
        Console::WriteLog("Launcher does not seem to be running!");
        return false;
    }

    if (!mem.Open(pid, SlimUtils::ProcessAccess::Full))
	{
		Console::WriteLog("Could not access launcher process!");
		return false;
	}

    auto mod = mem.GetModule(LAUNCHER_PROCESS);
    if (mod == nullptr) 
    {
        Console::WriteLog("Could not get launcher module!");
        return false;
    }
    
    int value = mem.Read<int>(mod->ptrBase + LAUNCHER_OFFSET);
    if (value == 1313756498) // RUNNING
    {
        Console::WriteLog("Activated");
        return true;
    } else if (value == 1162891086) // NOPE
    {
        Console::WriteLog("Launcher is running but user not logged in!");
        return false;
    } else 
    {
        Console::WriteLog("Invalid status reported from launcher");
        return false;
    }
}