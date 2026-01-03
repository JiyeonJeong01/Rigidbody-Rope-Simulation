#pragma once

#include "Define.h"

namespace VectorHelper
{
	static bool Is_Zero(const Vec3& v)
	{
		return v.x == 0.f && v.y == 0.f && v.z == 0.f;
	}

	static bool Is_NearlyZero(const Vec3& v, float eps = 1e-6f)
	{
		return fabsf(v.x) < eps &&
			fabsf(v.y) < eps &&
			fabsf(v.z) < eps;
	}

	static Vec3 Get_Normalized(const Vec3& vSrc)
	{
		Vec3 vTmp; 
		D3DXVec3Normalize(&vTmp, &vSrc);
		return vTmp;
	}

	static Vec3 CrossProduct(const Vec3& v1, const Vec3& v2)
	{
		Vec3 vTmp; 
		D3DXVec3Cross(&vTmp, &v1, &v2);
		return vTmp;
	}

	static Vec3 Right()
	{
		return Vec3(1.f, 0.f, 0.f);
	}

	static Vec3 Up()
	{
		return Vec3(0.f, 1.f, 0.f);
	}

	static Vec3 Look()
	{
		return Vec3(0.f, 0.f, 1.f);
	}

	static Vec3 Zero()
	{
		return Vec3(0.f, 0.f, 0.f);
	}



}
