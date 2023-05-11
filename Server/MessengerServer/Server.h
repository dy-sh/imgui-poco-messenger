// Copyright 2023 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <Poco/Mutex.h>

#include "ServerRoom.h"
#include "Protocol/Messages/ClientCreateRoomMessage.h"
#include "Protocol/Messages/ClientGetRoomsMessage.h"


struct Message;
struct ClientTextMessage;
struct ClientAuthorizeMessage;
struct ServerUser;
class ServerSocketHandler;

class Server
{
public:
    Server();
    std::vector<ServerUser*> GetAllAuthorizedUsers();

    void ReceiveMessage(Message* message, ServerSocketHandler* socket_handler);
    void OnSocketShutdown(ServerSocketHandler* socket_handler);

    void Broadcast(std::string message);

private:
    size_t last_user_id{0};
    int last_room_id{0};
    std::map<std::string, ServerUser*> users;
    Poco::FastMutex users_mutex;
    // std::vector<TextMessage> messages;
    void AuthorizeUser(ClientAuthorizeMessage& message, ServerSocketHandler* socket_handler);
    void ReceiveText(ClientTextMessage& message, ServerSocketHandler* socket_handler);
    void GetSerializedRooms(std::string& mess);
    void ReceiveCreateRoom(const ClientCreateRoomMessage& message, ServerSocketHandler* socket_handler);
    void ReceiveGetRooms(const ClientGetRoomsMessage& message, ServerSocketHandler* socket_handler);

    std::map<int, std::unique_ptr<ServerRoom>> rooms;
};
