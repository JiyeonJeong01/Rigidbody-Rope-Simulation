#pragma once
#include "pch.h"
class Collider;

typedef struct tagContactInfo
{
	Collider*			A;
	Collider*			B;
	Vec3					vResolveN_A;		// A가 겹침을 해결하는 방향
	Vec3					vPenetrateN_A;	// A가 침범하는 방향
	Vec3					vN_PlaneA;
	Vec3					vN_PlaneB;
	Vec3					vPoint;
	float					fDepth;
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
