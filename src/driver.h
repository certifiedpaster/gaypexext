#ifndef DRIVER_H
#define DRIVER_H

#include <windows.h>

class Driver 
{
private:
    bool _init;
    int _pid;
    HANDLE _drvhandle;

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

    void SendCommand(Command* command);
    bool UnsafeRead(int pid, uintptr_t source, uintptr_t destination, uintptr_t size);
    bool UnsafeWrite(int pid, uintptr_t source, uintptr_t destination, uintptr_t size);
public:
    void Init(int pid);

    template<typename T>
    T Read(uintptr_t address) 
    {
        T val = T();    
        if (!_init) 
            return val;

        UnsafeRead(_pid, address, (uintptr_t)&val, sizeof(T));
        return val;
    }

    template<typename T>
    void Write(uintptr_t address, T val) 
    {
        if (!_init) 
            return;

        UnsafeWrite(_pid, (uintptr_t)&val, address, sizeof(T));
    }
};

#endif