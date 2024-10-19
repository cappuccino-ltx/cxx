
.PHONY:all
all:udpserver udpclient

udpserver:udpserver.cpp
	g++ -o $@ $^ -std=c++11 -lpthread -g
udpclient:udpclient.cpp	
	g++ -o $@ $^ -std=c++11 -lpthread -g

.PHONY:clean
clean:
	rm -f udpserver
	rm -f udpclient