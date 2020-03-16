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
    uintptr_t studioHdr;
    
    uintptr_t viewAngles;
    uintptr_t cameraPos;
    uintptr_t vecOrigin;
    uintptr_t absVelocity;
    uintptr_t activeWeapon;
    uintptr_t boneClass;
    uintptr_t propName;
    uintptr_t bleedout;
    uintptr_t lifeState;
    uintptr_t teamNum;
    uintptr_t health;
    uintptr_t shield;
    uintptr_t flags;

    uintptr_t bulletSpeed;
    uintptr_t bulletGravity;
} Offsets;

typedef struct _Visuals 
{
    bool enabled;
    bool box;
    bool health;
    bool shield;
    bool showTarget;
    bool fovCircle;
} Visuals;

typedef struct _Aim 
{
    bool enabled;
    float maxfov;
    bool nopunch;
    int aimkey;
    int maxdistance;

    bool smooth;
    int divider;

    bool velocity;
    bool gravity;

    bool teamCheck;
    bool knockCheck;
} Aim;

typedef struct _Settings 
{
    Visuals visuals;
    Aim aim;
    int maxfps;
} Settings;

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

    Settings settings;
} GlobalVars;

extern GlobalVars* g_Vars;
extern Driver* g_Drv;

#define TESTBUILD true

#endif