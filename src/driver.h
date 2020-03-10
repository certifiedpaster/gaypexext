#ifndef DRIVER_H
#define DRIVER_H

#include <windows.h>

class Driver 
{
private:
    HANDLE _handle;
    ULONG _pid;

    typedef struct _DrvIO
    {
        ULONG   ProcID;
        ULONG64 pRemote;
        ULONG   Size;
        ULONG   Flag;
    } DrvIO, *P_DrvIO;
    typedef struct _DrvModInfo
    {
        ULONG64 ModBase;
        ULONG   ModSize;
    } DrvModInfo, *P_DrvModInfo;
    typedef struct _DrvAllocInfo
    {
        ULONG64 AllocBase;
        ULONG64 PhysBase;
        ULONG64 AllocMdl;
    } DrvAllocFreeInfo, *P_DrvAllocFreeInfo;

    void UnsafeRead(PVOID pTarget, PVOID pLocal, ULONG DataSize);
    void UnsafeWrite(PVOID pTarget, PVOID pLocal, ULONG DataSize);
    DrvAllocFreeInfo UnsafeAlloc(ULONG RegSize);
public:
    void Init();

};

#endif