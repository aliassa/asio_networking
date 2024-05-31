#include "main.h"
#include <iostream>
#include <chrono>
#include <thread>

std::vector<char> vBuffer(20 * 1024);

void GrabSomeData(asio::ip::tcp::socket& socket)
{
    socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
        [&](std::error_code ec, std::size_t length)
        {
            if(!ec)
            {
                std::cout << "\n\nRead " << length << " bytes\n\n";
                for(int i = 0; i < length; ++i)
                    std::cout << vBuffer[i];
                GrabSomeData(socket);
            }
        }
    );
}

int main(void)
{
    asio::error_code ec;
    asio::io_context context;
    asio::io_context::work idleWordk(context);
    std::thread thrContext = std::thread([&] { context.run(); });
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);
    asio::ip::tcp::socket socket(context);

    socket.connect(endpoint, ec);

    if(!ec)
    {
        std::cout << "Connected!\n";
    }
    else
    {
        std::cout << "Failed to connect " << ec.message() << '\n';
    }
    if(socket.is_open())
    {
        GrabSomeData(socket);

        std::string sRequest = 
            "GET /index.html HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Connection: clode\r\n\r\n";
        
        socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

        /*socket.wait(socket.wait_read);

        size_t bytes = socket.available();
        std::cout << "Bytes available: " << bytes << '\n'; 

        if(bytes > 0)
        {
            std::vector<char> vBuffer(bytes);
            socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);
            for(auto c : vBuffer)
                std::cout << c;
        }*/        
    }
    thrContext.join();
    return 0;
}