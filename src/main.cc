#include "charfix.h"
#include <windows.h>

#include "globals.h"
#include "overlay.h"
#include "console.h"
#include "utils.h"

GlobalVars* g_Vars;
Driver* g_Drv;

void OfflineOffsets() 
{
    // updated: 3/11/2020
    g_Vars->offsets.lastupdate = 1583943005;
    
    g_Vars->offsets.localPlayer = 0x1dea658;
    g_Vars->offsets.entityList = 0x1883c38;
    g_Vars->offsets.punchAngle = 0x2308;
    g_Vars->offsets.viewMatrix = 0x1b3bd0;

    g_Vars->offsets.absVelocity = 0x140;
    g_Vars->offsets.activeWeapon = 0x1944;
    g_Vars->offsets.boneClass = 0xee0;
    g_Vars->offsets.propName = 0x518;
    g_Vars->offsets.bleedout = 0x2590;
    g_Vars->offsets.lifeState = 0x730;
    g_Vars->offsets.teamNum = 0x3f0;
    g_Vars->offsets.health = 0x3e0;
    g_Vars->offsets.flags = 0x98;   
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{      
    g_Vars = (GlobalVars*)malloc(sizeof(GlobalVars));
    g_Vars->activated = false;
    g_Vars->shouldExit = false;
    g_Vars->ready = false;

    if (TESTBUILD) 
    {
        Console::WriteLog("Starting in debug mode...");
        OfflineOffsets();

        Console::WriteLog("Allocating and connecting to system console...");
        AllocConsole();
        freopen_s((FILE**)stdin,  "CONIN$", "r",  stdin);
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout); 
        
        g_Vars->activated = true;
    } else 
    {
        // TODO: server connection socket.io
        // load offsets from server
        
    }
    
    Console::WriteLog("Creating threads...");
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Overlay::Loop, nullptr, 0, nullptr);

    Console::WriteLog("Getting program info...");
    int pid = Utils::FindProcess(L"r5apex.exe");
    Console::WriteLog("Game PID is %i", pid);
    uintptr_t baseaddr = Utils::GetBase(pid, "r5apex.exe");
    Console::WriteLog("Game base address is %llx", baseaddr);
    g_Vars->apexBase = baseaddr;
    
    Console::WriteLog("Trying to connect to the driver...");
    g_Drv = new Driver();
    g_Drv->Init(pid);   

    Console::WriteLog("Using offsets from %s", Utils::UnixDate(g_Vars->offsets.lastupdate).c_str());
    g_Vars->ready = true;

    while (!g_Vars->shouldExit) 
    {
        Sleep(1000);
    }

    Console::WriteLog("Exit signal received. Exiting.");
    g_Vars->ready = false;
    Sleep(5000);

    return 0;
}