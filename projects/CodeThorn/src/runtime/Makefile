all: testruntimesystem

testruntimesystem: testruntimesystem.C runtime.h runtime.C
	g++ testruntimesystem.C -otestruntimesystem -lpthread

check: testruntimesystem
	./testruntimesystem

clean:
	rm -f testruntimesystem
