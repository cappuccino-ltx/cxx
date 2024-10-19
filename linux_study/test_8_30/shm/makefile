.PHONY:all
all:comm shmserver shmclient 

shmclient:client.cpp 
	g++ -o $@ $^
shmserver:server.cpp 
	g++ -o $@ $^
comm:comm.hpp
	g++ -o $@ $^

.PHONY:clean
clean:
	rm -f comm
	rm -f shmserver
	rm -f shmclient