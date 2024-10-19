

oj_server:oj_server.cc
	g++ -g -o $@ $^ -ljsoncpp -lpthread -lctemplate -std=c++11 -lmysqlclient

.PHONY:
clean:
	rm -rf oj_server
