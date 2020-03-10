#include <windows.h>
#include <cstdarg>
#include <cstdio>
#include <vector>
#include <string>
#include <ctime>
#include "globals.h"
#include "console.h"

std::vector<Console::TextData*> g_VecTextData;

std::string Console::GetTime() 
{
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    std::string str(buffer);

    return str;
}

void Console::Clean() 
{
    g_VecTextData.clear();
}

void Console::WriteLog(const char* text, ...) 
{
    char buffer[256];

    va_list argptr;
    va_start(argptr, text);
    vsnprintf(buffer, 255, text, argptr);
    va_end(argptr);

    std::string output = "[" + GetTime() + "] " + std::string(buffer);

    TextData* tx = (TextData*)malloc(sizeof(TextData));
    memcpy(tx->text, output.c_str(), 255);

    g_VecTextData.push_back(tx);
}