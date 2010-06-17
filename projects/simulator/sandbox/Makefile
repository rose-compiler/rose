all: sandbox test

sandbox: emulator.C simulate.C linuxcalls.C simulate.h memory.h elfload.C /home/willcock2/rose-svn-checkout/install/include/x86InstructionSemantics.h
	g++ -o sandbox emulator.C simulate.C linuxcalls.C elfload.C -I/home/willcock2/rose-svn-checkout/install/include -I/home/willcock2/boost-1.35-install64/include/boost-1_35 -L. -lrose

test: test.c
	gcc -m32 -o test test.c

clean:
	-rm -f sandbox test
