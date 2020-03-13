#ifndef ENTITY_H
#define ENTITY_H

#include "../globals.h"
#include "../sdk/structs.h"

class EntityWeapon 
{
private:
    uintptr_t _base;

public:
    EntityWeapon(uintptr_t base)
    {
        _base = base;
    }
    
    float BulletSpeed() 
    {
        return g_Drv->Read<float>(_base + g_Vars->offsets.bulletSpeed);
    }

    float BulletGravity() 
    {
        return g_Drv->Read<float>(_base + g_Vars->offsets.bulletGravity);
    }
};

class EntityPlayer 
{
private:
    uintptr_t _base;

    int BoneByHitBox(int hitbox) 
    {
		//DWORD64 Model = *(DWORD64*)(this + Off::StudioHdr);
        DWORD64 Model = g_Drv->Read<DWORD64>(_base + g_Vars->offsets.studioHdr);
		if (!Model) return -1;

		//get studio hdr
		//DWORD64 StudioHdr = *(DWORD64*)(Model + 8);
        DWORD64 StudioHdr = g_Drv->Read<DWORD64>(Model + 8);
		if (!StudioHdr) return -1;

		//get hitbox array
		//DWORD64 HitBoxsArray = StudioHdr + *(int*)(StudioHdr + 0xB4);
        DWORD64 HitBoxsArray = StudioHdr + g_Drv->Read<int>(StudioHdr + 0xB4);
		if (!HitBoxsArray) return -1;

		//get bone index
		//int Bone = *(int*)(HitBoxsArray + *(int*)(HitBoxsArray + 8) + (hitbox * 0x2C));
        int Bone = g_Drv->Read<int>(HitBoxsArray + g_Drv->Read<int>(HitBoxsArray + 8) + (hitbox * 0x2C));
		return ((Bone < 0) || (Bone > 255)) ? -1 : Bone;
    }
public:    
    EntityPlayer(uintptr_t base)    
    {
        _base = base;
    }
 
    int Health() 
    {
       return g_Drv->Read<int>(_base + g_Vars->offsets.health);
    }

    int Shield() 
    {
       return g_Drv->Read<int>(_base + g_Vars->offsets.shield);
    }

    Vector Position() 
    {
        return g_Drv->Read<Vector>(_base + g_Vars->offsets.vecOrigin);
    }

    uintptr_t Name() 
    {
        return g_Drv->Read<uintptr_t>(_base + g_Vars->offsets.propName);
    }

    Vector HitBoxPos(int HitBox)
	{
		//DWORD64 Bones = *(DWORD64*)(this + Off::BoneClass); 
        DWORD64 Bones = g_Drv->Read<DWORD64>(_base + g_Vars->offsets.boneClass);
		if (!Bones) return Vector(); Vector BoneOff;
		int Bone = BoneByHitBox(HitBox);
		if (Bone == -1) return Vector();

        //Matrix3x4* BoneMatrix = (Matrix3x4*)(Bones + (Bone * sizeof(Matrix3x4)));
        Matrix3x4 BoneMatrix = g_Drv->Read<Matrix3x4>(Bones + (Bone * sizeof(Matrix3x4)));
        BoneOff = { BoneMatrix._14, BoneMatrix._24, BoneMatrix._34 };
		
		return Position() + BoneOff;
	}

    Vector Camera() 
    {
        return g_Drv->Read<Vector>(_base + g_Vars->offsets.cameraPos);
    }

    Vector Viewangles() 
    {
        return g_Drv->Read<Vector>(_base + g_Vars->offsets.viewAngles);
    }

    void WriteViewangles(Vector angle) 
    {
        g_Drv->Write<Vector>(_base + g_Vars->offsets.viewAngles, angle);
    }

    EntityWeapon* ActiveWeapon() 
    {
        DWORD64 ActWeaponId = g_Drv->Read<DWORD64>(_base + g_Vars->offsets.activeWeapon) & 0xFFFF;
		if (ActWeaponId) 
        {
            uintptr_t pweapon = g_Drv->Read<uintptr_t>(g_Vars->apexBase + g_Vars->offsets.entityList + (ActWeaponId << 5));
            EntityWeapon* ewpn = new EntityWeapon(pweapon);
            return ewpn;
        }
        else 
        {
            return nullptr;
        }
    }

    Vector Velocity() 
    {
        return g_Drv->Read<Vector>(_base + g_Vars->offsets.absVelocity);
    }
};

#endif