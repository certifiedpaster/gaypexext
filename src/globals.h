#ifndef GLOBALS_H
#define GLOBALS_H

#include "driver.h"

typedef struct _GlobalVars 
{
    bool activated;
    char user[256];
    bool shouldExit;
    bool ready;
} GlobalVars;

extern GlobalVars* g_Vars;
extern Driver* g_Drv;

#define TESTBUILD true

#endif