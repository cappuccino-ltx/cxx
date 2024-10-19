.PHNOY:all
all:test 

test:test.cpp
	g++ -o $@ $^ -std=c++11 -lpthread -g

.PHONY:clean
clean:
	rm -f test