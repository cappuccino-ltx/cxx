
all:test_server test_client

test_server:test_server.cc
	g++ -o $@ $^
test_client:test_client.cc
	g++ -o $@ $^

test:test.cc
	g++ -o $@ $^

.PHONY:
clean:
	rm -rf test