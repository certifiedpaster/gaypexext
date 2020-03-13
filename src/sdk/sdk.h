#ifndef SDK_H
#define SDK_H

#include "structs.h"
#include "entity.h"
#include "w2s.h"

namespace SDK 
{
    Vector CalculateAngle(Vector src, Vector dst) 
    {
        Vector angle;
        Vector delta = src - dst;

        double hYp = sqrt((double)(delta.x * delta.x) + (delta.y * delta.y));
        angle.x = (float)(atan(delta.z / hYp) * 57.295779513082f);
        angle.y = (float)(atan(delta.y / delta.x) * 57.295779513082f);
        if (delta.x >= 0.0) angle.y += 180.0f;

        return angle;
    }

    Vector NormalizeAngles(Vector angle) 
    {
        while (angle.x > 89.0f)
            angle.x -= 180.0f;

        while (angle.x < -89.0f)
            angle.x += 180.0f;

        while (angle.y > 180.0f)
            angle.y -= 360.0f;

        while (angle.y < -180.0f)
            angle.y += 360.0f;

        return angle;
    }

    float GetFOV(Vector aimangle, Vector viewangle) 
    {
        Vector delta = aimangle - viewangle;
        delta = NormalizeAngles(delta);

        return (float)(sqrt(pow(delta.x, 2.0f) + pow(delta.y, 2.0f)));
    }
}

#endif