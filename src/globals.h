#ifndef GLOBALS_H
#define GLOBALS_H

#include "driver.h"

typedef struct _Offsets 
{
    int lastupdate;

    uintptr_t localPlayer;
    uintptr_t entityList;
    uintptr_t punchAngle;
    uintptr_t viewMatrix;
    uintptr_t viewRender;
    
    uintptr_t vecOrigin;
    uintptr_t absVelocity;
    uintptr_t activeWeapon;
    uintptr_t boneClass;
    uintptr_t propName;
    uintptr_t bleedout;
    uintptr_t lifeState;
    uintptr_t teamNum;
    uintptr_t health;
    uintptr_t flags;
} Offsets;

typedef struct _GlobalVars 
{
    bool activated;
    char user[256];
    bool shouldExit;
    bool ready;

    uintptr_t apexBase;
    Offsets offsets;

    int width;
    int height;
} GlobalVars;

extern GlobalVars* g_Vars;
extern Driver* g_Drv;

#define TESTBUILD true

#endif