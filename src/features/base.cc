#include "../globals.h"
#include "../console.h"
#include "base.h"
#include "../sdk/sdk.h"
#include "../overlay.h"
#include "../utils.h"

#define GREEN ImColor(ImVec4(0.0f, 1.0f, 0.0f, 1.0f))
#define RED ImColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f))
#define BLUE ImColor(ImVec4(0.0f, 0.0f, 1.0f, 1.0f))

void FeatureBase::Loop() 
{
    Utils::LimitFPS(g_Vars->settings.maxfps);
    
    if (!g_Vars->ready)
        return;
    
    uintptr_t plocalplayer = g_Drv->Read<uintptr_t>(g_Vars->apexBase + g_Vars->offsets.localPlayer);
    EntityPlayer* localplayer = new EntityPlayer(plocalplayer);
    Vector camera = localplayer->Camera();
    Vector viewangles = localplayer->Viewangles();

    uintptr_t aimtarget = 0;    
    float maxfov = 999.9f;  
    for (int i = 0; i < 100; i++) 
    {
        uintptr_t current = g_Drv->Read<uintptr_t>(g_Vars->apexBase + g_Vars->offsets.entityList + (i << 5));
        EntityPlayer* player = new EntityPlayer(current);
        
        if (!player)
            continue;

        // TODO: local-player check and entity type check

        if (plocalplayer == current)
            continue;

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

        if (g_Vars->settings.aim.enabled) 
        {
            Vector calcangle = SDK::CalculateAngle(camera, player->HitBoxPos(0));
            float fov = SDK::GetFOV(calcangle, viewangles);

            if (fov < maxfov && fov < g_Vars->settings.aim.maxfov) 
            {
                maxfov = fov;
                aimtarget = current;
            }
        }
    }

    if (g_Vars->settings.aim.enabled) 
    {
        if (aimtarget) 
        {
            if(GetKeyState(g_Vars->settings.aim.aimkey) & 0x8000) 
            {
                EntityPlayer* player = new EntityPlayer(aimtarget);
                Vector target = player->HitBoxPos(0);

                if (g_Vars->settings.aim.gravity || g_Vars->settings.aim.velocity) 
                {
                    EntityWeapon* active = localplayer->ActiveWeapon();
                    if (active) 
                    {
                        float BulletSpeed = active->BulletSpeed();
                        float BulletGrav = active->BulletGravity();

                        if (BulletSpeed > 1.f) 
                        {
                            Vector muzzle = localplayer->HitBoxPos(0); // TODO: use actual muzzle pos
                            if (g_Vars->settings.aim.gravity) 
                            {
                                float VerticalTime = SDK::Dist3D(target, muzzle) / BulletSpeed;
                                target.z += (750.f * BulletGrav * 0.5f) * (VerticalTime * VerticalTime);
                            }
                            if (g_Vars->settings.aim.velocity) 
                            {
                                float HorizontalTime = SDK::Dist3D(target, muzzle) / BulletSpeed;
                                target += (player->Velocity() * HorizontalTime);
                            } 
                        }     
                    }  
                }

                Vector calcangle = SDK::CalculateAngle(camera, target);
                Vector Delta = calcangle - viewangles;
                
                if (g_Vars->settings.aim.smooth && abs(Delta.x) > 0.009 && abs(Delta.y) > 0.009)
                {
                    Delta = Delta / ((float)g_Vars->settings.aim.divider / 100);
                }

                Vector SmoothedAngles = viewangles + Delta;

                if (g_Vars->settings.aim.nopunch) 
                {
                    Vector RecoilVec = g_Drv->Read<Vector>(plocalplayer + g_Vars->offsets.punchAngle);
                            
                    if (RecoilVec.x != 0 || RecoilVec.y != 0)
                    {
                        if (g_Vars->settings.aim.smooth)
                        {
                            RecoilVec = RecoilVec / ((float)g_Vars->settings.aim.divider / 100);
                        }
                        SmoothedAngles -= RecoilVec;
                    }
                }

                SmoothedAngles = SDK::NormalizeAngles(SmoothedAngles);
                localplayer->WriteViewangles(SmoothedAngles);
            }                    
        }            
    }
}