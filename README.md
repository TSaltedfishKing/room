# [room] A LiteLoaderBDS C++ Plugin
## 使用
### 指令如下
* `roomjoin [int room_id]` 【玩家权限】加入房间(room_id) 如果房间已满，则进入等候区
* `roomtp [int room_id room_id_sub]` 【管理员权限】加入房间(room_id) (room_id_sub) 无视最大人数设定
### 特殊物品如下
在游戏中，您可以使用铁砧亦或是其他命名工具，给任意物品命名，能够使用的物品，例如雪球

其中有如下物品名会有所的特殊公用【若无特殊说明，权限均为管理员】

**位置设定类**

* Set.waitingPosition
* Set.checkpoint
* Set.positionA
* Set.positionB

**修改/创建配置类**

* Modify A SubRoom
* Modify A Room
* Create A SubRoom
* Create A Room

### 特殊事件
当玩家加入服务器时，如果区域恰好为副本，那么玩家将会传送至副本的等候区内

特别的，当玩家具有`fmd`的tag时，玩家不会被传送，如果需要传送至等候区，可以尝试使用roomjoin来传送
## License
This repository is open source under the Apache License Version 2.0, January 2004.

Please refer to [the license file](LICENSE) for further information.

**This project used following libraries for development**

* [simpleini](https://github.com/brofield/simpleini)  MIT License
* [SDK-CPP](https://github.com/LiteLDev/SDK-CPP/) MIT License