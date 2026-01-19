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

	static float Get_Length(const Vec3& vSrc)
	{
		return D3DXVec3Length(&vSrc);
	}

	static float Get_LengthSq(const Vec3& vSrc)
	{
		return D3DXVec3LengthSq(&vSrc);
	}

	static Vec3 CrossProduct(const Vec3& v1, const Vec3& v2)
	{
		Vec3 vTmp; 
		D3DXVec3Cross(&vTmp, &v1, &v2);
		return vTmp;
	}

	static float DotProduct(const Vec3& v1, const Vec3& v2)
	{
		return D3DXVec3Dot(&v1, &v2);
	}

	static Vec3 HadamardProduct(const Vec3& v1, const Vec3& v2)
	{
        return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
	}

	static Vec3 TransformNormal(Vec3* pVec, Matrix* pMat)
	{
		Vec3 vTmp;
		D3DXVec3TransformNormal(&vTmp, pVec, pMat);
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
