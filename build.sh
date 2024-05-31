echo "building client"
g++  -g -std=c++20 -I./asio/include  SimpleClient.cpp -lncurses  -o client
echo "building server"
g++  -g -std=c++20 -I./asio/include  SimpleServer.cpp -lncurses  -o server
