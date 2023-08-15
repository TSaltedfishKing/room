#pragma once
#ifndef _ROOM_HPP_
#define _ROOM_HPP_

#include <llapi/mc/Vec3.hpp>
#include <llapi/mc/AABB.hpp>
#include "SimpleIni.h"
#include "utility/convert.hpp"

struct SubRoomInfo {
	int roomId;
	Vec3 checkpoint;
	AABB aabbPosition;
	int playerCount;
};
struct RoomInfo {
	int roomId;
	string description;
	int maxSubRoomPlayerCount;
	//int rule;
	Vec3 waitingPosition;
	std::vector<SubRoomInfo> subroomInfo;
};
//操作配置文件
namespace Room {

	//插件初始化
	void RoomInitalize();

	std::vector<RoomInfo> GetAllRoomInfo();
	std::vector<SubRoomInfo> GetAllSubRoomInfo(int room_id);
	string GetRoomDescription(int ID);
	Vec3 GetSubRoomCheckPoint(int ID, int ID_SUB);
	bool RoomExists(int ID);
	bool RoomExists(int ID, int ID_SUB);
	bool SetRoomInfo(int ID, string description);
	bool SetRoomInfo(int ID, int maxSubRoomPlayerCount);
	bool SetRoomInfo(int ID, Vec3 waitingPosition);
	bool SetRoomInfo(int ID, string description, Vec3 waitingPosition, std::vector<SubRoomInfo> subroomInfo);
	bool SetRoomInfo(int ID, string description, std::vector<SubRoomInfo> subroomInfo);
	bool SetRoomInfo(int ID, Vec3 waitingPosition, std::vector<SubRoomInfo> subroomInfo);
	bool SetSubRoomInfo(int ID, int ID_SUB, Vec3 checkpoint, Vec3 postionA, Vec3 postionB);
	bool SetSubRoomInfo(int ID, int ID_SUB, Vec3 checkpoint);
	bool SetSubRoomInfo(int ID, int ID_SUB, Vec3 postionA, Vec3 postionB);
	bool CreateRoom(int ID);
	bool CreateRoom(int ID, string description);
	bool CreateRoom(int ID, string description, Vec3 waitingPostion);
	bool CreateSubRoom(int ID, int ID_SUB, Vec3 checkpoint, Vec3 postionA, Vec3 postionB);
	bool DeleteRoom(int ID);
	bool DeleteSubRoom(int ID, int ID_SUB);
	
}
//操作变量roomInfoList
namespace RoomInfoController {
	//@return: 最佳子房间 如果不存在返回-1
	int GetBestSubRoom(std::vector<RoomInfo> roomInfoList, int ID);
	int GetRoomInfoIndex(std::vector<RoomInfo> roomInfoList, int room_id);
}
#endif // !_ROOM_HPP_