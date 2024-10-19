
.PHONY:all
all:TcpServer tcp_client

TcpServer:TcpServer.cpp
	g++ -o $@ $^ -std=c++11 
tcp_client:tcp_client.cpp
	g++ -o $@ $^ -std=c++11 

.PHONY:clean
clean:
	rm -f TcpServer
	rm -f tcp_client