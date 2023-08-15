/**
 * @file plugin.cpp
 * @brief The main file of the plugin
 */

#include <llapi/LoggerAPI.h>

#include "version.h"
#include "utility/convert.hpp"

#include <string>
#include "SimpleIni.h"
#include "room.hpp"
#include <llapi/FormUI.h>

#include <llapi/mc/CommandOrigin.hpp>
#include <llapi/mc/CommandOutput.hpp>
#include <llapi/mc/ItemStack.hpp>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Types.hpp>
#include <llapi/ParticleAPI.h>

#include <llapi/DynamicCommandAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/GlobalServiceAPI.h>
#include <llapi/ScheduleAPI.h>


 // We recommend using the global logger.
extern Logger logger;

std::vector<RoomInfo> _roomInfoList;
void LOAD_DATA() {
	_roomInfoList = Room::GetAllRoomInfo();
	for (size_t i = 0; i < _roomInfoList.size(); i++)
	{
		_roomInfoList[i].subroomInfo = Room::GetAllSubRoomInfo(_roomInfoList[i].roomId);
	}
}
//输出所有子房间人数到文本文件
void PrintPlayerCount() {
	//输出所有子房间人数到文本文件
	std::ofstream fout;
	fout.open("subroom.txt");
	for (size_t i = 0; i < _roomInfoList.size(); i++)
	{
		for (size_t j = 0; j < _roomInfoList[i].subroomInfo.size(); j++)
		{
			fout << std::to_string(_roomInfoList[i].roomId) + "-" + std::to_string(_roomInfoList[i].subroomInfo[j].roomId) + ":" + std::to_string(_roomInfoList[i].subroomInfo[j].playerCount) << std::endl;
		}
	}
	fout.close();
}
bool isPlayerContainTag(Player* player, string tag) {
	auto tags = player->getTags();
	for (size_t i = 0; i < tags.size(); i++)
	{
		if (tags[i] == "fmd") {
			return true;
		}
	}
	return false;
}
/**
 * @brief The entrypoint of the plugin. DO NOT remove or rename this function.
 *
 */
void PluginInit()
{
	Room::RoomInitalize();
	using ParamType = DynamicCommand::ParameterType;
	using ParamData = DynamicCommand::ParameterData;
	//@command setup

	//command 'roomjoin' =>room teleport
	{
		DynamicCommand::setup(
			"roomjoin", // The command
			"加入房间", // The description
			{ }, // The enumeration
			{ ParamData("room_id",ParamType::Int,false) }, // The parameters
			{ {"room_id"} }, // The overloads
			[](
				DynamicCommand const& command,
				CommandOrigin const& origin,
				CommandOutput& output,
				std::unordered_map<std::string, DynamicCommand::Result>& results
				) {
					int BestSubRoom = RoomInfoController::GetBestSubRoom(_roomInfoList, results["room_id"].get<int>());
					logger.info(std::to_string(BestSubRoom));
					if (BestSubRoom != -1)
					{
						Vec3 tppos = Room::GetSubRoomCheckPoint(results["room_id"].get<int>(), BestSubRoom);
						origin.getPlayer()->sendText("正在传送至 " + std::to_string(results["room_id"].get<int>()) + "-" + std::to_string(BestSubRoom));
						Global<Level>->executeCommand("tpp " + origin.getName() + " " + std::to_string(tppos.x) + " " + std::to_string(tppos.y) + " " + std::to_string(tppos.z));
						origin.getPlayer()->sendPlaySoundPacket("random.levelup", origin.getPlayer()->getPos(), 1, 1);
					}
					else
					{
						origin.getPlayer()->sendPlaySoundPacket("mob.villager.no", origin.getPlayer()->getPos(), 1, 1);
						int _Index = RoomInfoController::GetRoomInfoIndex(_roomInfoList, results["room_id"].get<int>());
						origin.getPlayer()->sendText("副本人数已满 正在传送至 " + std::to_string(results["room_id"].get<int>()) + " 的等候区");
						Global<Level>->executeCommand("tp " + origin.getPlayer()->getName() + " " + std::to_string(_roomInfoList[_Index].waitingPosition.x) + " " + std::to_string(_roomInfoList[_Index].waitingPosition.y) + " " + std::to_string(_roomInfoList[_Index].waitingPosition.z));
					};
			}, CommandPermissionLevel::Any // The callback function
		); 
		DynamicCommand::setup(
			"roomtp", // The command
			"传送至房间(Admin Only)", // The description
			{ }, // The enumeration
			{ ParamData("room_id",ParamType::Int,false),
			  ParamData("room_id_sub", ParamType::Int,false) }, // The parameters
			{ {"room_id","room_id_sub"} }, // The overloads
			[](
				DynamicCommand const& command,
				CommandOrigin const& origin,
				CommandOutput& output,
				std::unordered_map<std::string, DynamicCommand::Result>& results
				) {
					int room_id_sub = results["room_id_sub"].get<int>();
					Vec3 tppos = Room::GetSubRoomCheckPoint(results["room_id"].get<int>(), room_id_sub);
					origin.getPlayer()->sendText("正在传送至 " + std::to_string(results["room_id"].get<int>()) + std::to_string(room_id_sub));
					Global<Level>->executeCommand("tp " + origin.getName() + " " + std::to_string(tppos.x) + " " + std::to_string(tppos.y) + " " + std::to_string(tppos.z));
					origin.getPlayer()->sendPlaySoundPacket("random.levelup", origin.getPlayer()->getPos(), 1, 1);
			}, CommandPermissionLevel::GameMasters // The callback function
		);
	}
	Event::ServerStartedEvent::subscribe([](const Event::ServerStartedEvent& _) {
		LOAD_DATA();
		Schedule::repeat([&]() {
				auto players = Level::getAllPlayers();
				for (size_t i = 0; i < players.size(); i++)
				{
					for (size_t j = 0; j < _roomInfoList.size(); j++)
					{
						for (size_t k = 0; k < _roomInfoList[j].subroomInfo.size(); k++)
						{
							if (_roomInfoList[j].subroomInfo[k].aabbPosition.contains(players[i]->getAABB())) {
								_roomInfoList[j].subroomInfo[k].playerCount++;
								PrintPlayerCount();
							}
							else {}
						}

					}
				}
			}, 50);
		Schedule::repeat([&]() {LOAD_DATA();}, 100);
		Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& event) {
			auto player = event.mPlayer;
			//用循环判断玩家在哪个副本里
			int playerRoomVecId;
			int playerSubRoomVecId;
			for (size_t i = 0; i < _roomInfoList.size(); i++)
				for (size_t j = 0; j < _roomInfoList.size(); j++)
				{
					for (size_t k = 0; k < _roomInfoList[j].subroomInfo.size(); k++)
					{
						if (_roomInfoList[j].subroomInfo[k].aabbPosition.contains(player->getAABB())) {
							if (_roomInfoList[j].maxSubRoomPlayerCount < _roomInfoList[j].subroomInfo[k].playerCount) {
								if (isPlayerContainTag(player, "fmd")) {
									player->sendPlaySoundPacket("mob.villager.no", player->getPos(), 1, 1);
									player->sendText("副本人数已满 但由于您具有“fmd”标签，已被豁免 使用roomjoin指令可以在人满的时候传送至等候区");
								}
								else
								{
									
									player->sendPlaySoundPacket("mob.villager.no", player->getPos(), 1, 1);
									player->sendText("副本人数已满 正在传送至 " + std::to_string(_roomInfoList[j].roomId) + " 的等候区");
									Global<Level>->executeCommand("tp " + player->getName() + " " + std::to_string(_roomInfoList[j].waitingPosition.x) + " " + std::to_string(_roomInfoList[j].waitingPosition.y) + " " + std::to_string(_roomInfoList[j].waitingPosition.z));
								}
							}
							break;
						}
						else {}
					}

				}
			return true;
			});
		Event::PlayerUseItemEvent::subscribe([](const Event::PlayerUseItemEvent& event) {
			static std::unordered_map<std::string, Vec3> waitingPosition;
			static std::unordered_map<std::string, Vec3> checkpoint;
			static std::unordered_map<std::string, Vec3> postionA;
			static std::unordered_map<std::string, Vec3> postionB;
			//判断
			if (event.mPlayer->isOperator()) {
				if (event.mItemStack->getCustomName() == "Set.waitingPosition")
				{
					waitingPosition[event.mPlayer->getName()] = event.mPlayer->getFeetPosition();
				}
				if (event.mItemStack->getCustomName() == "Set.checkpoint")
				{
					checkpoint[event.mPlayer->getName()] = event.mPlayer->getFeetPosition();
				}

				if (event.mItemStack->getCustomName() == "Set.postionA")
				{
					postionA[event.mPlayer->getName()] = event.mPlayer->getFeetPosition();
				}
				if (event.mItemStack->getCustomName() == "Set.postionB")
				{
					postionB[event.mPlayer->getName()] = event.mPlayer->getFeetPosition();
				}
				if (event.mItemStack->getCustomName() == "Modify A SubRoom") {
					std::vector<string> room_id_List;
					for (const auto& roomInfo : _roomInfoList) {
						room_id_List.push_back(std::to_string(roomInfo.roomId));
					}
					string _string;
					if (checkpoint.count(event.mPlayer->getName()) != 0) {
						_string = "checkpoint: " + Vec3ToString(checkpoint[event.mPlayer->getName()])+"\n";
					}
					else
					{
						_string = "checkpoint not set yet\n";
					}
					if (postionA.count(event.mPlayer->getName()) != 0) {
						_string = _string + "postionA: " + Vec3ToString(postionA[event.mPlayer->getName()]) + "\n";
						if (postionA.count(event.mPlayer->getName()) != 0) {
							_string = _string + "postionB: " + Vec3ToString(postionB[event.mPlayer->getName()]);
						}
						else
						{
							_string = _string + "postionB not set yet\nPlease set postionA and postionB together";
						}
					}
					else
					{
						_string = _string + "postionA not set yet\n";
						if (postionA.count(event.mPlayer->getName()) != 0) {
							_string = _string + "postionB: " + Vec3ToString(postionB[event.mPlayer->getName()]);
						}
						else
						{
							_string = _string + "postionB not set yet\nPlease set postionA and postionB together";
						}
					}
					
					Form::CustomForm form2("Modify A SubRoom");
					form2.addLabel("label1", "INFOMATIONS")
						.addDropdown("room_id", "Room ID", room_id_List)
						.addInput("room_sub_id", "SubRoom ID")
						.addLabel("label2", "The things you will change")
						.addLabel("label3",_string)
						.addToggle("checkpoint", "Change the checkpoint?", true)
						.addToggle("postion", "Change the postionA and postionB?", true)

						.sendTo(event.mPlayer,
							[](Player* player, auto result)
							{
								if (result.empty())
									return;
								if (result["room_id"]->getString() == "") {
									player->sendText("Room ID is empty.");
									return;
								}
								if (Room::RoomExists(std::stoi(result["room_id"]->getString())) == false) {
									player->sendText("Room ID is invalid.");
									return;
								}
								if (result["room_sub_id"]->getString() == "") {
									player->sendText("SubRoom ID is empty.");
									return;
								}
								if (Room::RoomExists(std::stoi(result["room_id"]->getString()), std::stoi(result["room_sub_id"]->getString())) == false) {
									player->sendText("SubRoom ID is invalid.");
									return;
								}
								if (result["checkpoint"]->getBool()) {
									if (checkpoint.count(player->getName()) == 0) {
										player->sendText("checkpoint is empty.");
										return;
									}
									Room::SetSubRoomInfo(std::stoi(result["room_id"]->getString()), std::stoi(result["room_sub_id"]->getString()), checkpoint[player->getName()]);
								}
								if (result["postion"]->getBool()) {
									if (postionA.count(player->getName()) == 0) {
										player->sendText("postionA is empty.");
										return;
									}
									if (postionB.count(player->getName()) == 0) {
										player->sendText("postionB is empty.");
										return;
									}
									Room::SetSubRoomInfo(std::stoi(result["room_id"]->getString()), std::stoi(result["room_sub_id"]->getString()), postionA[player->getName()], postionB[player->getName()]);
								}
								//输出
								player->sendText("SUBROOM MODIFIED AND HERE ARE THE SUBROOM INFO");
								LOAD_DATA();
								player->sendText("Room ID: " + result["room_id"]->getString());
								player->sendText("SubRoom ID: " + std::to_string(result["room_sub_id"]->getInt()));
								if (result["checkpoint"]->getBool()) {
									player->sendText("checkpoint: " + Vec3ToString(checkpoint[player->getName()]));
								}
								if (result["postion"]->getBool()) {
									player->sendText("postionA: " + Vec3ToString(postionA[player->getName()]));
									player->sendText("postionB: " + Vec3ToString(postionB[player->getName()]));
								}
							});
				}
				if (event.mItemStack->getCustomName() == "Create A SubRoom") {
					std::vector<string> room_id_List;
					for (const auto& roomInfo : _roomInfoList) {
						room_id_List.push_back(std::to_string(roomInfo.roomId));
					}
					string _string;
					if (checkpoint.count(event.mPlayer->getName()) != 0) {
						_string = "checkpoint: " + Vec3ToString(checkpoint[event.mPlayer->getName()]) + "\n";
					}
					else
					{
						_string = "checkpoint not set yet, Please make all things set\n";
					}
					if (postionA.count(event.mPlayer->getName()) != 0) {
						_string = _string + "postionA: " + Vec3ToString(postionA[event.mPlayer->getName()]) + "\n";
						if (postionA.count(event.mPlayer->getName()) != 0) {
							_string = _string + "postionB: " + Vec3ToString(postionB[event.mPlayer->getName()]);
						}
						else
						{
							_string = _string + "postionB not set yet\nPlease make all things set";
						}
					}
					else
					{
						_string = _string + "postionA not set yet\n";
						if (postionA.count(event.mPlayer->getName()) != 0) {
							_string = _string + "postionB: " + Vec3ToString(postionB[event.mPlayer->getName()]);
						}
						else
						{
							_string = _string + "postionB not set yet\nPlease make all things set";
						}
					}

					Form::CustomForm form2("Create A SubRoom");
					form2.addLabel("label1", "INFOMATIONS")
						.addDropdown("room_id", "Room ID", room_id_List)
						.addInput("room_sub_id", "SubRoom ID")
						.addLabel("label2", "The subroom you will create")
						.addLabel("label3", _string)

						.sendTo(event.mPlayer,
							[](Player* player, auto result)
							{
								if (result.empty())
									return;
								if (result["room_id"]->getString() == "") {
									player->sendText("Room ID is empty.");
									return;
								}
								if (Room::RoomExists(std::stoi(result["room_id"]->getString())) == false) {
									player->sendText("Room ID is invalid.");
									return;
								}
								if (result["room_sub_id"]->getString() == "") {
									player->sendText("SubRoom ID is empty.");
									return;
								}
								if (Room::RoomExists(std::stoi(result["room_id"]->getString()), std::stoi(result["room_sub_id"]->getString()))) {
									player->sendText("SubRoom ID is already exist.");
									return;
								}
								{
									if (checkpoint.count(player->getName()) == 0) {
										player->sendText("checkpoint is empty.");
										return;
									}
									if (postionA.count(player->getName()) == 0) {
										player->sendText("postionA is empty.");
										return;
									}
									if (postionB.count(player->getName()) == 0) {
										player->sendText("postionB is empty.");
										return;
									}
								}
								Room::CreateSubRoom(std::stoi(result["room_id"]->getString()), std::stoi(result["room_sub_id"]->getString()), checkpoint[player->getName()], postionA[player->getName()], postionB[player->getName()]);
								//输出
								player->sendText("SUBROOM CREATED AND HERE ARE THE SUBROOM INFO");
								LOAD_DATA();
								player->sendText("Room ID: " + result["room_id"]->getString());
								player->sendText("SubRoom ID: " + std::to_string(result["room_sub_id"]->getInt()));
								player->sendText("checkpoint: " + Vec3ToString(checkpoint[player->getName()]));
								player->sendText("postionA: " + Vec3ToString(postionA[player->getName()]));
								player->sendText("postionB: " + Vec3ToString(postionB[player->getName()]));
							});
				}
				if (event.mItemStack->getCustomName() == "Create A Room") {
					if (waitingPosition.count(event.mPlayer->getName()) == 0) {
						event.mPlayer->sendText("waitingPosition is empty. Please set it first");
					}
					else {
						string _string = Vec3ToString(waitingPosition[event.mPlayer->getName()]);
						Form::CustomForm form2("Create A Room");
						form2.addLabel("label1", "INFOMATIONS")
							.addInput("room_id", "Room ID")
							.addInput("description", "Description")
							.addLabel("waitingPosition", "waitingPosition" + _string)
							.sendTo(event.mPlayer,
								[](Player* player, auto result)
								{
									if (result.empty())
										return;
									if (result["room_id"]->getString() == "") {
										player->sendText("Room ID is empty.");
										return;
									}
									if (Room::RoomExists(std::stoi(result["room_id"]->getString())) == true) {
										player->sendText("Room ID is already exist.");
										return;
									}
									Room::CreateRoom(std::stoi(result["room_id"]->getString()), result["description"]->getString(), waitingPosition[player->getName()]);
									//输出
									player->sendText("ROOM CREATED AND HERE ARE THE ROOM INFO");
									LOAD_DATA();
									player->sendText("Room ID: " + result["room_id"]->getString());
									player->sendText("Description: " + result["description"]->getString());
									player->sendText("waitingPosition: " + Vec3ToString(waitingPosition[player->getName()]));
								});

					}
					
				}
				if (event.mItemStack->getCustomName() == "Modify A Room") {
					std::vector<string> room_id_List;
					for (const auto& roomInfo : _roomInfoList) {
						room_id_List.push_back(std::to_string(roomInfo.roomId));
					}
					string _string = Vec3ToString(waitingPosition[event.mPlayer->getName()]);
					Form::CustomForm form2("Modify A Room");
					form2.addLabel("label1", "INFOMATIONS")
						.addDropdown("room_id", "Room ID", room_id_List)
						.addToggle("Toggle1", "Change the Description?", false)
						.addInput("description", "Description")
						.addToggle("Toggle2", "Change the waitingPosition?", false)
						.addLabel("waitingPosition", "waitingPosition" + _string)
						.sendTo(event.mPlayer,
							[](Player* player, auto result)
							{
								if (result["Toggle1"]->getBool())
								{
									Room::SetRoomInfo(std::stoi(result["room_id"]->getString()), result["description"]->getString());
								}
								if (result["Toggle2"]->getBool())
								{
									Room::SetRoomInfo(std::stoi(result["room_id"]->getString()), waitingPosition[player->getName()]);
								}
								//输出
								player->sendText("ROOM MODIFIED AND HERE ARE THE ROOM INFO");
								LOAD_DATA();
								player->sendText("Room ID: " + result["room_id"]->getString());
								if (result["Toggle1"]->getBool())
									player->sendText("Description: " + result["description"]->getString());
								if (result["Toggle2"]->getBool())
									player->sendText("waitingPosition: " + Vec3ToString(waitingPosition[player->getName()]));
							});
				}
			}
			return true;
			});
		return true;
		});
}