#pragma once
#include <iostream>
#include <windows.h>

#ifndef DXCall
#define DXCall(x)																\
if (FAILED(x))																	\
{																				\
	char line_number[32];														\
	sprintf_s(line_number, "%u", __LINE__);										\
	OutputDebugStringA("Error in: ");											\
	OutputDebugStringA(__FILE__);												\
	OutputDebugStringA("\nLine:");												\
	OutputDebugStringA(line_number);											\
	OutputDebugStringA("\n");													\
	OutputDebugStringA(#x);														\
	OutputDebugStringA("\n");													\
	__debugbreak();																\
}

#endif

#ifndef NAME_D3D12_OBJECT
#define NAME_D3D12_OBJECT(object, name)	object->SetName(name);OutputDebugString(L"::D3D12 Object Created:  ");OutputDebugString(name);OutputDebugStringA("\n");

#endif

#ifndef NAME_D3D12_OBJECT_Indexed
#define NAME_D3D12_OBJECT_Indexed(object, n, name)								\
{																				\
	wchar_t full_name[128];														\
	if (swprintf_s(full_name, L"%s[%u]", name, n) > 0)							\
	{																			\
		object->SetName(full_name);												\
		OutputDebugString(L"::D3D12 Object Created:  ");						\
		OutputDebugString(full_name); OutputDebugStringA("\n");						\
	}																			\
}
#endif
namespace Engine
{
	constexpr uint32_t frame_buffer_count = { 3 };
}