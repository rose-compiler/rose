
ROSS_FLAGS=-I${ROSS_HOME}/include -D_GNU_SOURCE -g -Wall \
    -DUSE_AVL_TREE -DAVL_NODE_COUNT=262144 -DRAND_NORMAL \
    -DROSS_timing -DROSS_QUEUE_splay -DROSS_RAND_clcg4 \
    -DROSS_NETWORK_mpi -DROSS_CLOCK_amd64 -DROSS_GVT_mpi_allreduce \
    -DARCH_x86_64

all: testruntimesystem

testruntimesystem: testruntimesystem.C runtime.h runtime.C 
	mpiCC runtime.C testruntimesystem.C -lpthread ${ROSS_FLAGS} -I/usr/include/mpich2 -otestruntimesystem -I/usr/include -I${MPI_HOME}/include

check: testruntimesystem
	./testruntimesystem

clean:
	rm -f testruntimesystem
