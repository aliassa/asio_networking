#include "net_message.h"
#include "net_client.h"
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



class CustomClient : public net::client_interface<CustomMsgType>
{
public:
    void PingServer()
    {
        net::message<CustomMsgType> msg;
        msg.header.id = CustomMsgType::ServerPing;
        auto timeNow = std::chrono::system_clock::now();
        msg << timeNow;
        Send(msg);
    }
    void MessageAll()
	{
		net::message<CustomMsgType> msg;
		msg.header.id = CustomMsgType::MessageAll;		
		Send(msg);
	}
};

int main()
{
    CustomClient c;
    c.Connect("localhost", 60'000);
    bool bQuit = false;
    
    initscr();            // Initialize the window
    cbreak();             // Disable line buffering
    noecho();             // Don't echo input
    nodelay(stdscr, TRUE); // Non-blocking input
    keypad(stdscr, TRUE); // Enable function keys

    while (!bQuit)
    {
        int key = getch();
        if(key != ERR)
        {
            if(key == 'a') c.PingServer();
            if(key == 'b') c.MessageAll();
            if(key == 'q') bQuit = true;

        }

        if(c.IsConnected())
        {

            if( !c.Incoming().empty())
            {
                auto msg = c.Incoming().pop_front().msg;

                switch (msg.header.id)
                {
                case CustomMsgType::ServerAccept:
				{
					// Server has responded to a ping request				
					std::cout << "Server Accepted Connection\n";
				}
				break;
                case CustomMsgType::ServerPing:
                {
                    auto timeNow = std::chrono::system_clock::now();
                    std::chrono::system_clock::time_point timeThen;
                    msg >> timeThen;
                    std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
                }
                break;
                case CustomMsgType::ServerMessage:
				{
					// Server has responded to a ping request	
					uint32_t clientID;
					msg >> clientID;
					std::cout << "Hello from [" << clientID << "]\n";
				}
				break;
                default:
                    break;
                }
            }
        }
        else
        {
            std::cout << "Server Down\n";
            bQuit = true;
        }
    }
    return 0;
}