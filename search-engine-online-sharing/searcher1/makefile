
http_server:http_server.cc
	g++ -o $@ $^ -ljsoncpp -lmysqlclient -lpthread

test:test.cc
	g++ -o $@ $^ -ljsoncpp -lmysqlclient -lpthread

test1:test.cc
	g++ -c $< -ljsoncpp -lmysqlclient -lpthread

clean:
	rm -f test http_server