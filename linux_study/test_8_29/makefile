.PHONY:all
all:test.exe dup2

test.exe:openTest.cpp
	g++ -o $@ $^ -g
dup2:dup2.cpp
	g++ -o $@ $^

.PHONY:clean
clean:
	rm -f test.exe
	rm -f dup2
	rm -f *.txt