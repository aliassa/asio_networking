#include "net_server.h"
#include "net_common.h"
#include <ncurses.h>
#include <unistd.h>

enum class CustomMsgType : uint32_t
{
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage
};

class CustomSever : public net::server_interface<CustomMsgType>
{
public:
    CustomSever(uint32_t port) : net::server_interface<CustomMsgType>(port)
    {}

protected:
    bool OnClientConnect(std::shared_ptr<net::connection<CustomMsgType>> client) override
	{
		net::message<CustomMsgType> msg;
		msg.header.id = CustomMsgType::ServerAccept;
		client->Send(msg);
		return true;
	}

    virtual void OncClientDisconnect(std::shared_ptr<net::connection<CustomMsgType>> client)
    {
        std::cout << "Removing client [" << client->getId() << "]\n";
    }
    void OnMessage(std::shared_ptr<net::connection<CustomMsgType>> client, net::message<CustomMsgType>& msg) override
    {
        switch (msg.header.id)
        {
        case CustomMsgType::ServerPing:
        {
            std::cout << "[" << client->getId() << "]: Server Ping\n";
            //bounce message
            client->Send(msg); 
            /* code */
        }
        break;
        case CustomMsgType::MessageAll:
		{
			std::cout << "[" << client->getId() << "]: Message All\n";

			// Construct a new message and send it to all clients
			net::message<CustomMsgType> msg;
			msg.header.id = CustomMsgType::ServerMessage;
			msg << client->getId();
			MessageAllClients(msg, client);

		}
		break;
        
        default:
            break;
        }
    }

};

int main()
{
    CustomSever server(60'000);
    server.Start();
    while ((1))
    {
        server.Update(-1, true);
    }

    return 0;
}