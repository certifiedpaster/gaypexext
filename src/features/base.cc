#include "../globals.h"
#include "../console.h"
#include "base.h"
#include "../sdk/sdk.h"
#include "../overlay.h"

void FeatureBase::Loop() 
{
    if (!g_Vars->ready)
        return;
    
    for (int i = 0; i < 80; i++) 
    {
        uintptr_t current = g_Drv->Read<uintptr_t>(g_Vars->apexBase + g_Vars->offsets.entityList + (i << 5));
        EntityPlayer* player = new EntityPlayer(current);
        
        if (!player)
            continue;

        Vector origin = player->Position();
        Vector screen;
        bool w2s = SDK::World2Screen(origin, screen);
        if (w2s) 
        {
            Render::EasyText(ImVec2(screen.x, screen.y), ImColor(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)), "test");
        }
    }
}