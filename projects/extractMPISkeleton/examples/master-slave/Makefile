ALL: io
SHELL = /bin/sh
io: io.c
	mpicc -o io io.c 
profile.alog: io.c
	mpicc -o io.log -mpilog io.c 
	mpirun -np 4 io.log
	/bin/mv io.log_profile.log profile.alog
clean:
	/bin/rm -f io io.o io.log
