ALL: jacobi
SHELL = /bin/sh
jacobi: jacobi.c
	mpicc -o jacobi jacobi.c -lm 
profile.alog: jacobi.c
	mpicc -o jacobi.log -mpilog jacobi.c -lm
	mpirun -np 4 jacobi.log
	/bin/mv jacobi.log_profile.log profile.alog
clean:
	/bin/rm -f jacobi jacobi.o jacobi.log
