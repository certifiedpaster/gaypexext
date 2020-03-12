#ifndef W2S_H
#define W2S_H

#include "../globals.h"
#include "structs.h"

namespace SDK 
{
    bool World2Screen(const Vector& Position, Vector& ScreenPosition)
    {
        DWORD64 ViewRender = g_Drv->Read<DWORD64>(g_Vars->apexBase + g_Vars->offsets.viewRender); 
        if (!ViewRender) return false;

        //Matrix4x4* m_Matrix = *(Matrix4x4**)(ViewRender + Off::ViewMatrix); 
        Matrix4x4 m_Matrix = g_Drv->Read<Matrix4x4>(g_Drv->Read<uintptr_t>(ViewRender + g_Vars->offsets.viewMatrix)); 

        //rintf("%f %f %f %f\n", m_Matrix.m[0][0], m_Matrix.m[0][1], m_Matrix.m[0][2], m_Matrix.m[0][3]);

        float w = 0.f;
        ScreenPosition.x = (m_Matrix.m[0][0] * Position.x) + (m_Matrix.m[0][1] * Position.y) + (m_Matrix.m[0][2] * Position.z + m_Matrix.m[0][3]);
        ScreenPosition.y = (m_Matrix.m[1][0] * Position.x) + (m_Matrix.m[1][1] * Position.y) + (m_Matrix.m[1][2] * Position.z + m_Matrix.m[1][3]);
        w = (m_Matrix.m[3][0] * Position.x) + (m_Matrix.m[3][1] * Position.y) + (m_Matrix.m[3][2] * Position.z + m_Matrix.m[3][3]);

        if (w < 0.01f)
            return false;

        float invw = 1.f / w;
        ScreenPosition.x *= invw;
        ScreenPosition.y *= invw;

        float x = g_Vars->width / 2.f;
        float y = g_Vars->height / 2.f;
        x += .5f * ScreenPosition.x * g_Vars->width + .5f;
        y -= .5f * ScreenPosition.y * g_Vars->height + .5f;
        ScreenPosition.x = x; ScreenPosition.y = y; 
        return true;
    }
}

#endif