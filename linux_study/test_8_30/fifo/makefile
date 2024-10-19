
.PHONY:all
all:anonyPipe user server

anonyPipe:anonyPipe.cpp
	g++ -o $@ $^

user:namedPipe_user.cpp
	g++ -o $@ $^
server:namedPipe_server.cpp
	g++ -o $@ $^

.PHNOY:clean
clean:
	rm -f anonyPipe
	rm -f user
	rm -f server
	rm -f myfifo