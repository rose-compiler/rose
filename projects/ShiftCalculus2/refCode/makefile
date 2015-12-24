# Input of MPI generator is the generated file from the sequential DSL compiler
#  rose_laplacian_lite_v4.cpp test_mpi_v1.cpp are identical
#
# Output of the MPI generator is the manual translated reference files rose_test_mpi_v1.cpp 
# laplacian_mpi_v4_wrt.cpp and rose_test_mpi_v1.cpp are identical in the beginning
#
clean:
	rm -rf *.o *.out
# 3rd version using intuitive file name and more runtime support
#
#rose_test_mpi_v1.cpp
check: test_mpi_v1.out 
	mpirun -np 4 ./test_mpi_v1.out
test_mpi_v1.out: rose_test_mpi_v1.cpp
	mpicxx -std=c++11 -I../. -I../runtime  $< ../Box.cpp ../runtime/xomp_mpi.c -o $@
# 2nd version using runtime support	
#check: laplacian_mpi_wrt.out
#	mpirun -np 4 ./laplacian_mpi_wrt.out
# original version without runtime calls
#check: laplacian_mpi.out
#	mpirun -np 4 ./laplacian_mpi.out
#laplacian_mpi.out: laplacian_mpi_v4.cpp
#	mpicxx -std=c++11 -I../. $< ../Box.cpp -o $@

#laplacian_mpi_wrt.out: laplacian_mpi_v4_wrt.cpp
#	mpicxx -std=c++11 -I../. -I../runtime $< ../Box.cpp ../runtime/xomp_mpi.c -o $@
#----------------------- input file test ---------------------
# reference sequential code generated from DSL
# It is also the input code for MPI code generator
rose_laplacian_lite_v4.o:rose_laplacian_lite_v4.cpp
	g++ -std=c++11 -I../. -c rose_laplacian_lite_v4.cpp
