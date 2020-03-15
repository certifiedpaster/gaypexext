#ifndef SDK_H
#define SDK_H

#include "structs.h"
#include "entity.h"
#include "w2s.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
#ifndef M_RADPI
#define M_RADPI	57.295779513082f
#endif

namespace SDK 
{
    Vector CalculateAngle(Vector Src, Vector Dst) 
    {
        Vector Delta = Src - Dst, AimAngles;
		float Hyp = sqrtf(powf(Delta.x, 2.f) + powf(Delta.y, 2.f));
		AimAngles.y = atanf(Delta.y / Delta.x) * M_RADPI;
		AimAngles.x = atanf(Delta.z / Hyp) * M_RADPI;
		if (Delta.x >= 0.f) AimAngles.y += 180.f;
		AimAngles.z = 0.f; return AimAngles;
    }

    float Vec3Length(Vector& Src) 
    {
		return sqrtf(powf(Src.x, 2.f) + powf(Src.y, 2.f) + powf(Src.z, 2.f));
	}

    Vector ClampAngles(Vector Ang) 
    {
        if (Ang.x < -89.f) Ang.x = -89.f;
		if (Ang.x > 89.f) Ang.x = 89.f;
		while (Ang.y < -180.f) Ang.y += 360.f;
		while (Ang.y > 180.f) Ang.y -= 360.f;
		Ang.z = 0.f;
        return Ang;
    }

    float GetFOV(Vector aimangle, Vector viewangle) 
    {
        Vector delta = aimangle - viewangle;
        delta = ClampAngles(delta);

        return (float)(sqrt(pow(delta.x, 2.0f) + pow(delta.y, 2.0f)));
    }

    float FastSQRT(float x) 
    {
		union { int i; float x; } u;
		u.x = x; u.i = (u.i >> 1) + 0x1FC00000;
		u.x = u.x + x / u.x; return .25f * u.x + x / u.x;
	}

    float Dist3D(const Vector& Src, const Vector& Dst) 
    {
		return FastSQRT(powf(Src.x - Dst.x, 2.f) + powf(Src.y - Dst.y, 2.f) + powf(Src.z - Dst.z, 2.f));
	}
}

#endif