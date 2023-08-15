#include <string>
#include <llapi/mc/Vec3.hpp>
#include <llapi/mc/AABB.hpp>
#include "SimpleIni.h"
#include "room.hpp"
#include "version.h"
#include <filesystem>
#include <fstream>
#include <llapi/LoggerAPI.h>
extern Logger logger;
namespace Room {
	void RoomInitalize() {
		//如果文件夹不存在就创建
		if (!std::filesystem::exists(PLUGIN_CONFG_PATH.c_str()))
		{
			std::filesystem::create_directory(PLUGIN_CONFG_PATH.c_str());
		}
		else {}
		//文件不存在就创建
		if (!std::filesystem::exists(PLUGIN_CONFG_FILE.c_str()))
		{
			std::ofstream file(PLUGIN_CONFG_FILE.c_str());
			file.close();
		}
		else {}
	}
	std::vector<RoomInfo> GetAllRoomInfo() {
		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		std::vector<RoomInfo> roomInfoList;

		CSimpleIniW::TNamesDepend sections;
		ini.GetAllSections(sections);

		for (const auto& section : sections) {
			RoomInfo roomInfo;

			roomInfo.roomId = std::stoi(section.pItem);
			roomInfo.description = WideStringToString(ini.GetValue(section.pItem, L"description"));
			roomInfo.maxSubRoomPlayerCount = std::stoi(ini.GetValue(section.pItem, L"maxSubRoomPlayerCount"));
			roomInfo.waitingPosition = StringToVec3(WideStringToString(ini.GetValue(section.pItem, L"waitingPosition")));
			roomInfo.subroomInfo = GetAllSubRoomInfo(roomInfo.roomId);
			roomInfoList.push_back(roomInfo);
		}

		return roomInfoList;
	}

	std::vector<SubRoomInfo> GetAllSubRoomInfo(int room_id) {
		//如果文件不存在就创建
		if (!std::filesystem::exists((PLUGIN_CONFG_FILE + ".sub." + std::to_string(room_id) + ".ini").c_str()))
		{
			std::ofstream file((PLUGIN_CONFG_FILE + ".sub." + std::to_string(room_id) + ".ini").c_str());
			file.close();
		}
		CSimpleIniW ini;
		ini.LoadFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(room_id) + ".ini").c_str());
		std::vector<SubRoomInfo> subroomInfoList;

		CSimpleIniW::TNamesDepend sections;
		ini.GetAllSections(sections);

		for (const auto& section : sections) {
			SubRoomInfo subroomInfo;
			subroomInfo.roomId = std::stoi(section.pItem);
			subroomInfo.checkpoint = StringToVec3(WideStringToString(ini.GetValue(section.pItem, L"checkpoint")));
			subroomInfo.aabbPosition = pos2AABB(
				StringToVec3(WideStringToString(ini.GetValue(section.pItem, L"postionA"))),
				StringToVec3(WideStringToString(ini.GetValue(section.pItem, L"postionB")))
			);
			subroomInfo.playerCount = 0;
			subroomInfoList.push_back(subroomInfo);
		}
		return subroomInfoList;
	}

	string GetRoomDescription(int ID) {
		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			return WideStringToString(ini.GetValue(StringToWideString(std::to_string(ID)), L"description"));
		}
		else
		{
			return "";
		}
	}
	Vec3 GetSubRoomCheckPoint(int ID, int ID_SUB) {
		CSimpleIniW ini;
		ini.LoadFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(ID) + ".ini").c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			return StringToVec3(WideStringToString(ini.GetValue(StringToWideString(std::to_string(ID_SUB)), L"checkpoint")));
		}
		else
		{
			return Vec3(0, 0, 0);
		}
	}
	bool RoomExists(int ID) {
		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			return true;
		}
		else
		{
			return false;
		}
	}
	bool RoomExists(int ID, int ID_SUB) {
		CSimpleIniW ini;
		ini.LoadFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(ID) + ".ini").c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID_SUB))) != -1) {
			return true;
		}
		else
		{
			return false;
		}
	}
	bool SetRoomInfo(int ID, string description) {
		auto room_id = StringToWideString(std::to_string(ID));
		auto _description = StringToWideString(description);

		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			ini.SetValue(room_id, L"description", _description);
			ini.SaveFile(PLUGIN_CONFG_FILE.c_str());
			return true;
		}
		else
		{
			return false;
		}
	}
	bool SetRoomInfo(int ID, int maxSubRoomPlayerCount) {
		auto room_id = StringToWideString(std::to_string(ID));
		auto _maxSubRoomPlayerCount = StringToWideString(std::to_string(maxSubRoomPlayerCount));

		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			ini.SetValue(room_id, L"maxSubRoomPlayerCount", _maxSubRoomPlayerCount);
			ini.SaveFile(PLUGIN_CONFG_FILE.c_str());
			return true;
		}
		else {
			return false;
		}
	}
	bool SetRoomInfo(int ID, Vec3 waitingPosition) {
		auto room_id = StringToWideString(std::to_string(ID));
		auto _waitingPostion = StringToWideString(Vec3ToString(waitingPosition));

		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			ini.SetValue(room_id, L"waitingPostion", _waitingPostion);
			ini.SaveFile(PLUGIN_CONFG_FILE.c_str());
			return true;
		}
		else {
			return false;
		}
	}
	bool SetRoomInfo(int ID, string description, int maxSubRoomPlayerCount, Vec3 waitingPosition) {
		auto room_id = StringToWideString(std::to_string(ID));
		auto _description = StringToWideString(description);
		auto _maxSubRoomPlayerCount = StringToWideString(std::to_string(maxSubRoomPlayerCount));
		auto _waitingPostion = StringToWideString(Vec3ToString(waitingPosition));

		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			ini.SetValue(room_id, L"description", _description);
			ini.SetValue(room_id, L"maxSubRoomPlayerCount", _maxSubRoomPlayerCount);
			ini.SetValue(room_id, L"waitingPostion", _waitingPostion);
			ini.SaveFile(PLUGIN_CONFG_FILE.c_str());
			return true;
		}
		else {
			return false;
		}
	}
	bool SetRoomInfo(int ID, string description, Vec3 waitingPosition, std::vector<SubRoomInfo> subroomInfo)
	{
		auto room_id = StringToWideString(std::to_string(ID));
		auto _description = StringToWideString(description);

		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			ini.SetValue(room_id, L"description", _description);
			ini.SetValue(room_id, L"waitingPostion", StringToWideString(Vec3ToString(waitingPosition)));
			ini.SaveFile(PLUGIN_CONFG_FILE.c_str());
			return true;
		}
		else
		{
			return false;
		}
	}
	bool SetRoomInfo(int ID, string description, std::vector<SubRoomInfo> subroomInfo)
	{
		auto room_id = StringToWideString(std::to_string(ID));
		auto _description = StringToWideString(description);

		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			ini.SetValue(room_id, L"description", _description);
			ini.SaveFile(PLUGIN_CONFG_FILE.c_str());
			return true;
		}
		else
		{
			return false;
		}
	}
	bool SetRoomInfo(int ID, Vec3 waitingPosition, std::vector<SubRoomInfo> subroomInfo)
	{
		auto room_id = StringToWideString(std::to_string(ID));

		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			ini.SetValue(room_id, L"waitingPostion", StringToWideString(Vec3ToString(waitingPosition)));
			ini.SaveFile(PLUGIN_CONFG_FILE.c_str());
			return true;
		}
		else
		{
			return false;
		}
	}

	bool SetSubRoomInfo(int ID, int ID_SUB, Vec3 checkpoint, Vec3 postionA, Vec3 postionB)
	{
		auto room_id_sub = StringToWideString(std::to_string(ID_SUB));
		CSimpleIniW ini;
		ini.LoadFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(ID) + ".ini").c_str());
		if (ini.GetSectionSize(room_id_sub) != -1) {
			ini.SetValue(room_id_sub, L"checkpoint", StringToWideString(Vec3ToString(checkpoint)));
			ini.SetValue(room_id_sub, L"postionA", StringToWideString(Vec3ToString(postionA)));
			ini.SetValue(room_id_sub, L"postionB", StringToWideString(Vec3ToString(postionB)));
			ini.SaveFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(ID) + ".ini").c_str());
			return true;
		}
		else
		{
			return false;
		}
	}
	bool SetSubRoomInfo(int ID, int ID_SUB, Vec3 checkpoint)
	{
		auto room_id_sub = StringToWideString(std::to_string(ID_SUB));
		CSimpleIniW ini;
		ini.LoadFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(ID) + ".ini").c_str());
		if (ini.GetSectionSize(room_id_sub) != -1) {
			ini.SetValue(room_id_sub, L"checkpoint", StringToWideString(Vec3ToString(checkpoint)));
			ini.SaveFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(ID) + ".ini").c_str());
			return true;
		}
		else
		{
			return false;
		}
	}
	bool SetSubRoomInfo(int ID, int ID_SUB, Vec3 postionA, Vec3 postionB)
	{
		auto room_id_sub = StringToWideString(std::to_string(ID_SUB));
		CSimpleIniW ini;
		ini.LoadFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(ID) + ".ini").c_str());
		if (ini.GetSectionSize(room_id_sub) != -1) {
			ini.SetValue(room_id_sub, L"postionA", StringToWideString(Vec3ToString(postionA)));
			ini.SetValue(room_id_sub, L"postionB", StringToWideString(Vec3ToString(postionB)));
			ini.SaveFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(ID) + ".ini").c_str());
			return true;
		}
		else
		{
			return false;
		}
	}
	bool CreateRoom(int ID) {
		auto room_id = StringToWideString(std::to_string(ID));

		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			ini.SaveFile(PLUGIN_CONFG_FILE.c_str());
			return true;
		}
		else
		{
			return false;
		}
	}
	bool CreateRoom(int ID, string description) {
		auto room_id = StringToWideString(std::to_string(ID));
		auto _description = StringToWideString(description);

		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			ini.SetValue(room_id, L"description", _description);
			ini.SaveFile(PLUGIN_CONFG_FILE.c_str());
			return true;
		}
		else
		{
			return false;
		}
	}
	bool CreateRoom(int ID, string description, Vec3 waitingPostion) {
		auto room_id = StringToWideString(std::to_string(ID));
		auto _description = StringToWideString(description);
		auto _waitingPostion = StringToWideString(Vec3ToString(waitingPostion));

		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) == -1) {
			ini.SetValue(room_id, L"description", _description);
			ini.SetValue(room_id, L"waitingPostion", _waitingPostion);
			ini.SaveFile(PLUGIN_CONFG_FILE.c_str());
			return true;
		}
		else
		{
			return false;
		}
	}
	bool CreateSubRoom(int ID, int ID_SUB, Vec3 checkpoint, Vec3 postionA, Vec3 postionB) {
		auto room_id_sub = StringToWideString(std::to_string(ID_SUB));
		CSimpleIniW ini;
		ini.LoadFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(ID) + ".ini").c_str());
		if (ini.GetSectionSize(room_id_sub) == -1) {
			ini.SetValue(room_id_sub, L"checkpoint", StringToWideString(Vec3ToString(checkpoint)));
			ini.SetValue(room_id_sub, L"postionA", StringToWideString(Vec3ToString(postionA)));
			ini.SetValue(room_id_sub, L"postionB", StringToWideString(Vec3ToString(postionB)));
			ini.SaveFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(ID) + ".ini").c_str());
			return true;
		}
		else
		{
			return false;
		}
	}

	bool DeleteRoom(int ID) {
		auto room_id = StringToWideString(std::to_string(ID));
		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			ini.Delete(room_id, NULL);
			ini.SaveFile(PLUGIN_CONFG_FILE.c_str());
			return true;
		}
		else {
			return false;
		}
	}
	bool DeleteSubRoom(int ID, int ID_SUB) {
		auto room_id = StringToWideString(std::to_string(ID) + "." + std::to_string(ID_SUB));
		CSimpleIniW ini;
		ini.LoadFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(ID) + ".ini").c_str());
		if (ini.GetSectionSize(StringToWideString(std::to_string(ID))) != -1) {
			ini.Delete(room_id, NULL);
			ini.SaveFile((PLUGIN_CONFG_FILE + ".sub." + std::to_string(ID) + ".ini").c_str());
			return true;
		}
		else {
			return false;
		}
	}
	
}
namespace RoomInfoController {
	//roomInfo 根据room_id获取数组下标
	int GetRoomInfoIndex(std::vector<RoomInfo> roomInfoList, int room_id)
	{
		for (int i = 0; i < roomInfoList.size(); i++)
		{
			if (roomInfoList[i].roomId == room_id)
			{
				return i;
			}
		}
		return -1;
	}
	int GetBestSubRoom(std::vector<RoomInfo> roomInfoList, int ID) {
		auto room_id = StringToWideString(std::to_string(ID));
		CSimpleIniW ini;
		ini.LoadFile(PLUGIN_CONFG_FILE.c_str());
		for (const auto& subroomInfo : roomInfoList[GetRoomInfoIndex(roomInfoList, ID)].subroomInfo) {
			if (subroomInfo.playerCount < roomInfoList[GetRoomInfoIndex(roomInfoList, ID)].maxSubRoomPlayerCount) {
				return subroomInfo.roomId;
			}
		}
		return -1;
	}
}
