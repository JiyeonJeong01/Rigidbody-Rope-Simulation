#pragma once

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

}
