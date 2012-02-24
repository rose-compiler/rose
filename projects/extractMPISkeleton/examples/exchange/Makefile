ALL: exchng
SHELL = /bin/sh

exchng: exchng.c
	mpicc -o exchng exchng.c

profile.alog: exchng.c
	mpicc -o exchng.log -mpilog exchng.c 
	mpirun -np 4 exchng.log
	/bin/mv exchng.log_profile.log profile.alog

clean:
	/bin/rm -f exchng exchng.o exchng.log

