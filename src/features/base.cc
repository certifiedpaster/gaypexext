#include "../globals.h"
#include "../console.h"
#include "base.h"
#include "../sdk/sdk.h"
#include "../overlay.h"

#define GREEN ImColor(ImVec4(0.0f, 1.0f, 0.0f, 1.0f))
#define RED ImColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f))
#define BLUE ImColor(ImVec4(0.0f, 0.0f, 1.0f, 1.0f))

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

        // TODO: local-player check and entity type check

        if (g_Vars->settings.visuals.enabled) 
        {
            Vector head;
            Vector feet;
            if (SDK::World2Screen(player->HitBoxPos(0), head) && SDK::World2Screen(player->Position(), feet)) 
            {
                float Height = (head.y - feet.y), Width = Height / 2.f;
                if (g_Vars->settings.visuals.box) 
                {       
                    Render::DrawBox(RED, feet.x - (Width / 2.f), feet.y, Width, Height);
                }
                
                int width = 2;
                if (g_Vars->settings.visuals.health) 
                {
                    int health = player->Health();
                    int px = 10;
                    Render::Progress(feet.x + (Width / 2.f) - px, feet.y, width, Height, health);
                }
                if (g_Vars->settings.visuals.shield) 
                {
                    int shield = player->Shield();
                    int px = 5;
                    Render::Progress(feet.x - (Width / 2.f) + px, feet.y, width, Height, shield);
                }
            }
        }
    }
}