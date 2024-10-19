
.PHNOY:all
all:sigpending sigprocmask sigaction signal test mykill alarm 

sigpending:sigpending.cpp
	g++ -o $@ $^

sigprocmask:sigprocmask.cpp
	g++ -o $@ $^
sigaction:sigaction.cpp
	g++ -o $@ $^
signal:signal.cpp
	g++ -o $@ $^
test:test.cpp
	g++ -o $@ $^

mykill:mykill.cpp
	g++ -o $@ $^
alarm:alarm.cpp
	g++ -o $@ $^

.PHONY:clean
clean:
	rm -f sigprocmask
	rm -f sigpending
	rm -f sigaction
	rm -f signal
	rm -f test
	rm -f mykill
	rm -f alarm