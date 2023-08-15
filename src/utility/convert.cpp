#include "convert.hpp"
#include <windows.h>
#include <locale.h>
std::string Vec3ToString(const Vec3& vec) {
	std::stringstream ss;
	ss << "(" << std::fixed << std::setprecision(2) << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return ss.str();
}

Vec3 StringToVec3(const std::string& str) {
	Vec3 vec;
	std::istringstream iss(str);
	char discard;

	iss >> discard; // Discard the opening parenthesis '('
	float floatX, floatY, floatZ;
	iss >> floatX;
	iss >> discard; // Discard the comma ','
	iss >> floatY;
	iss >> discard; // Discard the comma ','
	iss >> floatZ;
	iss >> discard; // Discard the closing parenthesis ')'

	vec.x = floatX;
	vec.y = floatY;
	vec.z = floatZ;

	return vec;
}


const wchar_t* StringToWideString(const std::string& str) {
	int string_length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	wchar_t* wideStr = new wchar_t[string_length];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wideStr, string_length);
	return wideStr;
}
std::string WideStringToString(const wchar_t* wideStr) {
	int string_length = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);
	std::string str;
	str.resize(string_length);
	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, &str[0], string_length, nullptr, nullptr);
	return str;
}

AABB pos2AABB(const Vec3& pos1, const Vec3& pos2) {
	AABB aabb;
	aabb.min.x = std::min(pos1.x, pos2.x);
	aabb.min.y = std::min(pos1.y, pos2.y);
	aabb.min.z = std::min(pos1.z, pos2.z);

	aabb.max.x = std::max(pos1.x, pos2.x);
	aabb.max.y = std::max(pos1.y, pos2.y);
	aabb.max.z = std::max(pos1.z, pos2.z);

	return aabb;
}