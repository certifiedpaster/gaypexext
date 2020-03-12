#include "../globals.h"
#include "../console.h"
#include "base.h"
#include "../sdk/sdk.h"

void FeatureBase::Loop() 
{
    if (!g_Vars->ready)
        return;
    
    for (int i = 0; i < 80; i++) 
    {
        uintptr_t current = g_Drv->Read<uintptr_t>(g_Vars->apexBase + g_Vars->offsets.entityList + (i << 5));
        EntityPlayer* player = new EntityPlayer(current);
        
        int health = player->Health();
        if (health > 0) 
        {
            Console::WriteLog("pl health: %i", health);
        }      
    }
}