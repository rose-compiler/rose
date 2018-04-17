// #include <upc.h>
// #include <upc_collective.h>
// #include <upc_io.h>

#define NELEMS 10
#define BLK_SIZE 3
#define IO_FILE_NAME "upc_check_test_ip_op_file.upc"

// shared int *scatter_A;
// shared [] int *gather_B;

// shared [BLK_SIZE] signed char reduce_C_A[NELEMS*THREADS];
// shared [BLK_SIZE] signed char reduce_C_A[THREADS];
shared [BLK_SIZE] signed char reduce_C_A[NELEMS*THREADS];
