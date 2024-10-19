.PHONY:all
all:cfile sfile

cfile:cfile.cpp 
	g++ -o $@ $^

sfile:sfile.cpp 
	g++ -o $@ $^

.PHONY:clean
clean:
	rm -f cfile
