#pragma once
#include "pch.h"
class Collider;
class Transform;

typedef struct tagAxisMask
{
    bool bX = false;
    bool bY = false;
    bool bZ = false;
}AXIS_MASK;

typedef struct tagBody
{
    bool    bActive = true;
    BODY_TYPE eBodyType = DYNAMIC;
    GEOMETRY_TYPE eGeoType = GEOMETRY_TYPE::G_END;

    float   fMass = 1.f;
    float   fInvMass = 1.f; // invMass

    float   fDrag = 0.f;
    float   fAngularDrag = 0.f;

    float   fRestitution = 0.f;
    float   fFriction = 1.f;

	bool	bGravity = true;

	Vec3	vDimension, vDimensionCenter;

    Matrix  matInertiaTensor;
    Matrix  matInvInertiaTensor;

    Vec3    vCOM = { 0,0,0 };
    Vec3    vLinearVel = { 0,0,0 };
    Vec3    vAngularVel = { 0,0,0 };

    Vec3    vForceAccum = { 0,0,0 };
    Vec3    vTorqueAccum = { 0,0,0 };

	Transform* pTransform;

    AXIS_MASK   tRotationLock;
    AXIS_MASK   tPositionLock;
}BODY;

typedef struct tagContactInfo
{
	Collider*	A;
	Collider*	B;
	Vec3		vResolveN_A;		// A가 겹침을 해결하는 방향
	Vec3		vPenetrateN_A;	// A가 침범하는 방향
	Vec3		vN_PlaneA;
	Vec3		vN_PlaneB;
	Vec3		vPoint;
	float		fDepth;
}CONTACT_INFO;

typedef struct tagPairKey
{
    uint32_t aKey;
    uint32_t bKey;

	explicit tagPairKey(uint32_t a, uint32_t b)
    {
	    if ( a > b)
	    {
			aKey = b;
			bKey = a;
	    }
		else
		{
			aKey = a;
			bKey = b;
		}
    }

    bool operator==(const tagPairKey& other) const
    {
        return aKey == other.aKey && bKey == other.bKey;
    }
}PAIR_KEY;

typedef struct PairKeyHash
{
	size_t operator()(const PAIR_KEY& k) const
	{
		return (static_cast<size_t>(k.aKey) << 32) ^ k.bKey;
	}
}PAIR_KEY_HASH;

typedef struct tagSpringJoingInfo
{
	
}SPRING_JOINT_INFO;
