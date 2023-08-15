#pragma once
#ifndef _CONVERT_HPP_
#define _CONVERT_HPP_

#include <string.h>
#include <llapi/mc/Vec3.hpp>
#include <llapi/mc/AABB.hpp>


std::string Vec3ToString(const Vec3& vec);
Vec3 StringToVec3(const std::string& str);
const wchar_t* StringToWideString(const std::string& str);
std::string WideStringToString(const wchar_t* wideStr);

AABB pos2AABB(const Vec3& pos1, const Vec3& pos2);
#endif // !_CONVERT_HPP_