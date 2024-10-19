

all:test_pro test_con

test_pro:test_pro.cc
	g++ -o $@ $^ -lpthread

test_con:test_con.cc
	g++ -o $@ $^ -lpthread

.PHONY:
clean:cleanexe cleanfile

cleanexe:
	rm -rf test_pro test_con

cleanfile:
	rm -rf myfifo