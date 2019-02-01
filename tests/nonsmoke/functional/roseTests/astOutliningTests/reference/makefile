all: jacobi.out

#.c.so:
#	gcc -fpic -shared -lc $< 

SO_Files = rose_jacobi_lib.so

rose_jacobi_lib.so: rose_jacobi_lib.c
	gcc -fpic -shared -lc $<  -o $@

jacobi.out: rose_jacobi.c $(SO_Files) autotuning_lib.c
	gcc -I. -L. -O0 -g $< autotuning_lib.c -o $@ -ldl -Wl,-rpath,./. -lm
#	gcc -I. -L. -O0 -g $< autotuning_lib.c -o $@ -lOUT_1_jacobi_191 -ldl -Wl,-rpath,./. -lm
clean:
	rm -rf *.so *.out
check: all
	./jacobi.out
