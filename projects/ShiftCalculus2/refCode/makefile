clean:
	rm -rf *.o *.out
check: laplacian_mpi.out
	mpirun -np 4 ./laplacian_mpi.out
laplacian_mpi.out: laplacian_mpi_v4.cpp
	mpicxx -std=c++11 -I../. $< ../Box.cpp -o $@

laplacian_mpi_wrt.out: laplacian_mpi_wrt.cpp
	mpicxx -std=c++11 -I../. -I../runtime $< ../Box.cpp ../runtime/xomp_mpi.c -o $@
