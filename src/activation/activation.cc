#include "SlimMem.h"
#include "activation.h"
#include "../console.h"
#include "../vmprotect.h"

SlimUtils::SlimMem mem;
#define LAUNCHER_PROCESS EW(L"llauncher.exe")
#define LAUNCHER_OFFSET 0x55d20

bool Activation::Activate() 
{
    ProtectStart();
    
    DWORD pid;
    
    Console::WriteLog(E("Activating..."));
    if (!SlimUtils::SlimMem::GetPID(LAUNCHER_PROCESS, &pid)) 
    {
        Console::WriteLog(E("Launcher does not seem to be running!"));
        return false;
    }

    if (!mem.Open(pid, SlimUtils::ProcessAccess::Full))
	{
		Console::WriteLog(E("Could not access launcher process!"));
		return false;
	}

    auto mod = mem.GetModule(LAUNCHER_PROCESS);
    if (mod == nullptr) 
    {
        Console::WriteLog(E("Could not get launcher module!"));
        return false;
    }
    
    int value = mem.Read<int>(mod->ptrBase + LAUNCHER_OFFSET);
    if (value == 1313756498) // RUNNING
    {
        Console::WriteLog(E("Activated"));
        return true;
    } else if (value == 1162891086) // NOPE
    {
        Console::WriteLog(E("Launcher is running but user not logged in!"));
        return false;
    } else 
    {
        Console::WriteLog(E("Invalid status reported from launcher"));
        return false;
    }

    ProtectEnd();
}