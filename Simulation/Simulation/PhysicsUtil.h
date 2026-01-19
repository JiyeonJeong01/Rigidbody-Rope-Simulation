#pragma once
#include "Physics_Struct.h"

namespace PhysicsUtil
{
    void Set_StaticBody(BODY& b)
    {
        b.eType = STATIC;
        b.fInvMass = 0.f;
        ZeroMemory(&b.matInvInertiaTensor, sizeof(b.matInvInertiaTensor));
        b.bGravity = false;
    }

    void Set_DynamicBody(BODY& b)
    {
        b.eType = DYNAMIC;
        b.fMass = (b.fMass <= 0.f ? 1.f : b.fMass);
        b.fInvMass = 1.f / b.fMass;
        b.bGravity = true;
    }

    void Set_KinematicBody(BODY& b)
    {
        b.eType = KINEMATIC;
        b.fInvMass = 0.f;
        ZeroMemory(&b.matInvInertiaTensor, sizeof(b.matInvInertiaTensor));
        b.vForceAccum = {};
        b.vTorqueAccum = {};
        b.bGravity = false;
    }

    inline void ApplyPositionLock(BODY& b)
    {
        if (b.tPositionLock.bX)
        { b.vLinearVel.x = 0.f; b.vForceAccum.x = 0.f; }
        if (b.tPositionLock.bY)
        { b.vLinearVel.y = 0.f; b.vForceAccum.y = 0.f; }
        if (b.tPositionLock.bZ)
        { b.vLinearVel.z = 0.f; b.vForceAccum.z = 0.f; }
    }

    inline void ApplyRotationLock(BODY& b)
    {
        if (b.tRotationLock.bX)
        { b.vAngularVel.x = 0.f; b.vTorqueAccum.x = 0.f; }
        if (b.tRotationLock.bY)
        { b.vAngularVel.y = 0.f; b.vTorqueAccum.y = 0.f; }
        if (b.tRotationLock.bZ)
        { b.vAngularVel.z = 0.f; b.vTorqueAccum.z = 0.f; }
    }

}
