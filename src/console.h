#ifndef CONSOLE_H
#define CONSOLE_H

#include <vector>
#include <string>
#include <thread>
#include <mutex>

namespace Console 
{
    typedef struct _TextData 
    {
        char text[256];
    } TextData;
    
    std::string GetTime();
    void WriteLog(const char* text, ...);
    void Clean();
};

extern std::vector<Console::TextData*> g_VecTextData;

#endif

