.PHONY:all
all: cal_server cal_client

cal_server:cal_server.cpp
	g++ -o $@ $^ -std=c++11 -lpthread -ljsoncpp
cal_client:cal_client.cpp
	g++ -o $@ $^ -std=c++11 -ljsoncpp

.PHONY:clean
clean:
	rm -f cal_server
	rm -f cal_client