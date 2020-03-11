#ifndef DRIVER_H
#define DRIVER_H

#include <windows.h>

class Driver 
{
private:
    HANDLE _handle;
    ULONG _pid;
    bool _init;

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
    void Init(int pid);

    template<typename T>
    T Read(uintptr_t address) 
    {
        T val = T();    
        if (!_init) 
            return val;

        UnsafeRead((PVOID)address, (PVOID)&val, sizeof(T));
        return val;
    }

    template<typename T>
    void Write(uintptr_t address, T val) 
    {
        if (!_init) 
            return;

        UnsafeWrite((PVOID)address, (PVOID)&val, sizeof(T));
    }
};

#endif