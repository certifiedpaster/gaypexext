#include <windows.h>
#include "driver.h"
#include "console.h"
#include "vmprotect.h"
#include "globals.h"

typedef struct _Command
{
    int action; // action to do

    int pid1; // client
    int pid2; // target
    int tid; // target thread id

    uintptr_t source;
    uintptr_t destination;
    uintptr_t size;

    int* status; // return status
} Command;
#define IOCTL_SENDCOMMAND CTL_CODE(FILE_DEVICE_UNKNOWN, 0xDEAD, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

void Driver::SendCommand(Driver::Command* command) 
{   
    DeviceIoControl(_drvhandle, IOCTL_SENDCOMMAND, command, sizeof(Command), command, sizeof(Command), NULL, NULL);
}

bool Driver::UnsafeRead(int pid, uintptr_t source, uintptr_t destination, uintptr_t size) 
{    
    int status = 0;

    Command* c = new Command();
    c->action = 1;
    c->pid2 = pid;
    c->pid1 = GetCurrentProcessId();
    c->status = &status;
    c->source = source;
    c->destination = destination;
    c->size = size;

    SendCommand(c);

    return (status == 20);
}

bool Driver::UnsafeWrite(int pid, uintptr_t source, uintptr_t destination, uintptr_t size)
{
    int status = 0;

    Command* c = new Command();
    c->action = 2;
    c->pid2 = pid;
    c->pid1 = GetCurrentProcessId();
    c->status = &status;
    c->source = source;
    c->destination = destination;
    c->size = size;

    SendCommand(c);

    return (status == 20);
}

void Driver::Init(int pid) 
{
    ProtectStart();
    
    Console::WriteLog(E("Initializing driver connection..."));

    if (!_drvhandle) 
    {
        for (int i = 0; i < 10; i++)
        {
            wchar_t buffer[20];
            wsprintfW(buffer, EW(L"\\\\.\\PhysicalDrive%i"), i);
            _drvhandle = CreateFileW(buffer, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (!(_drvhandle == INVALID_HANDLE_VALUE || !_drvhandle))
            {
                Console::WriteLog(E("Driver handle: %p"), _drvhandle);
                break;
            }
        }
        if (_drvhandle == INVALID_HANDLE_VALUE || !_drvhandle)
        {
            Console::WriteLog(E("Failed to connect to the driver"));
        }
    }

    _init = true;
    _pid = pid;
    Console::WriteLog(E("Connection initialized"));

    ProtectEnd();
}