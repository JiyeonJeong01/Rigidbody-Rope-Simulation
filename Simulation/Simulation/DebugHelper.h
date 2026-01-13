#pragma once
#include <iomanip>
#include <iostream>

#include "Define.h"

namespace DebugHelper
{
	static void Print_Vec3(const wchar_t* str, const Vec3& v)
	{
		std::wcout << std::fixed << std::setprecision(8)
			<< std::left << std::setw(20)
			<< str << L" : "
			<< v.x << L", "
			<< std::left << std::setw(12)
			<< v.y << L", "
			<< std::left << std::setw(12)
			<< v.z << std::endl;
	}

	static void Print_Float(const wchar_t* str, const float& f)
	{
		std::wcout << std::fixed << std::setprecision(8)
			<< std::left << std::setw(20)
			<< str << L" : " << f << std::endl;
	}

	static void Print_String(const wchar_t* str)
	{
		std::wcout  << str << std::endl;
	}
}

