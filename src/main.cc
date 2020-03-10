#include "charfix.h"
#include <windows.h>

#include "globals.h"
#include "overlay.h"
#include "console.h"

GlobalVars* g_Vars;
Driver* g_Drv;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    g_Vars = (GlobalVars*)malloc(sizeof(GlobalVars));
    g_Vars->activated = false;
    g_Vars->shouldExit = false;
    g_Vars->ready = false;

    if (TESTBUILD) 
    {
        Console::WriteLog("Starting in debug mode...");
        g_Vars->activated = true;
    } else 
    {
        // TODO: server connection socket.io
    }
    
    Console::WriteLog("Creating threads...");
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Overlay::Loop, nullptr, 0, nullptr);

    Console::WriteLog("Trying to connect to the driver...");
    g_Drv = new Driver();
    g_Drv->Init();

    while (!g_Vars->shouldExit) 
    {
        Sleep(1000);
    }

    Console::WriteLog("Exit signal received. Exiting.");
    Sleep(5000);

    return 0;
}