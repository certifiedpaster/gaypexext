#include <windows.h>
#include "driver.h"
#include "console.h"

#define IOCTL_READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x9372, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
void Driver::UnsafeRead(PVOID pTarget, PVOID pLocal, ULONG DataSize)
{
	if (!DataSize) return;
	DrvIO OutData = { _pid, (ULONGLONG)pTarget, DataSize };
	DeviceIoControl(_handle, IOCTL_READ, &OutData, sizeof(OutData), pLocal, DataSize, NULL, NULL);
}

#define IOCTL_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1730, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
void Driver::UnsafeWrite(PVOID pTarget, PVOID pLocal, ULONG DataSize)
{
	if (!DataSize) return;
	DrvIO OutData = { _pid, (ULONGLONG)pTarget, DataSize };
	DeviceIoControl(_handle, IOCTL_WRITE, &OutData, sizeof(OutData), pLocal, DataSize, NULL, NULL);
}

#define IOCTL_ALLOC CTL_CODE(FILE_DEVICE_UNKNOWN, 0x9261, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
Driver::DrvAllocFreeInfo Driver::UnsafeAlloc(ULONG RegSize)
{
	DrvIO OutData = { _pid, 0, RegSize }; DrvAllocFreeInfo Addr = { 0 };
	DeviceIoControl(_handle, IOCTL_ALLOC, &OutData, sizeof(OutData), &Addr, sizeof(Addr), NULL, NULL);
	return Addr;
}

void Driver::Init() 
{
    HANDLE drvhandle = CreateFileW(L"\\\\.\\GpuEnergyDrv", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (drvhandle == INVALID_HANDLE_VALUE || !drvhandle) 
    {
        Console::WriteLog("Failed to open driver handle!");
    } else 
    {
        _handle = drvhandle;
        Console::WriteLog("Handle: %p", _handle);
    }
}

