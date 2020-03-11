#include "../globals.h"
#include "base.h"
#include "../sdk/sdk.h"

void FeatureBase::Loop() 
{
    for (int i = 0; i < 80; i++) 
    {
        uintptr_t current = g_Drv->Read<uintptr_t>(g_Vars->offsets.entityList + (i << 5));

    }
}