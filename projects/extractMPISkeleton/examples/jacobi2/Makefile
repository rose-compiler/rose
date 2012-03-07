ALL: jacobi2
SHELL = /bin/sh
jacobi2: jacobi2.c
	mpicc -o jacobi2 jacobi2.c -lm
profile.alog: jacobi2.c
	mpicc -o jacobi2.log -mpilog jacobi2.c -lm
	mpirun -np 4 jacobi2.log
	/bin/mv jacobi2.log_profile.log profile.alog
clean:
	/bin/rm -f jacobi2 jacobi2.o jacobi2.log
