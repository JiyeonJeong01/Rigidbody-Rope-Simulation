#pragma once

#include <random>

#include "d3dx9.h"

namespace MathHelper
{
	static void Euler_ToQuaternion(const Vec3& vEuler, Vec4& vQuaternion)
	{
		float pitch = D3DXToRadian(vEuler.x); // X
		float yaw = D3DXToRadian(vEuler.y); // Y
		float roll = D3DXToRadian(vEuler.z); // Z

		D3DXQUATERNION q;
		D3DXQuaternionRotationYawPitchRoll(&q, yaw, pitch, roll);

		vQuaternion = Vec4(q.x, q.y, q.z, q.w);
	}

	static bool Float_InRange(const float& fV, const float& fMin, const float& fMax)
	{
		return fV >= fMin && fV <= fMax;
	}

	static std::mt19937 m_gen(std::random_device{}());
	static float Random_Float(const float& fMin, const float& fMax)
	{
		std::uniform_real_distribution<float> dist(fMin, fMax);
		return dist(m_gen);
	}
	static int Random_Int(const int& iMin, const int& iMax)
	{
		std::uniform_real_distribution<float> dist(iMin, iMax);
		return dist(m_gen);
	}

}
