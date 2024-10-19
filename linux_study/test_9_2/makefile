.PHONY:all
all:pthread

pthread:pthread.cpp
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -f pthread