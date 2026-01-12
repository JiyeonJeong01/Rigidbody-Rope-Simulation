#pragma once
#include <iomanip>
#include <iostream>

#include "Define.h"

namespace DebugHelper
{
	static void Print_Vec3(const wchar_t* str, const Vec3& v)
	{
		std::wcout << std::fixed << std::setprecision(2)
			<< str << L" : "
			<< v.x << L", "
			<< v.y << L", "
			<< v.z << std::endl;
	}

	static void Print_Float(const wchar_t* str, const float& f)
	{
		std::wcout << std::fixed << std::setprecision(2) << str << L" : "<< f << std::endl;
	}

	static void Print_String(const wchar_t* str)
	{
		std::wcout  << str << std::endl;
	}
}

