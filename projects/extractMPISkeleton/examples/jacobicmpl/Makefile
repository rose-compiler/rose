ALL: jacobicmpl
SHELL = /bin/sh

jacobicmpl: jacobicmpl.c
	mpicc -o jacobicmpl jacobicmpl.c -lm 
profile.alog: jacobicmpl.c
	mpicc -o jacobicmpl.log -mpilog jacobicmpl.c -lm
	mpirun -np 4 jacobicmpl.log
	/bin/mv jacobicmpl.log_profile.log profile.alog
clean:
	/bin/rm -f jacobicmpl jacobicmpl.o jacobicmpl.log
