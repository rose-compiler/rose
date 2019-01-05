#include<stdio.h>
#include<stdlib.h>

#include <mpi.h>

#define T       double
#define MPI_T   MPI_DOUBLE

int global_arraySize;
int numberOfIterations;

/*
    variables for MPI
*/
int myid;
int nproc;
double center_time;
double boundary_time;
double total_time;

#define Index2D(i, j, nx, ny) ((i) + (j)*nx) 

#define Index3D(i, j, k, nx, ny, nz) ((i) + (j)*nx + (k)*nx*ny)

void print_usage()
{
    printf("Usage of MPI Stencil on Manycore Architecture:\n");
    printf("$> stencil <iteration> <num_of_elem in each direction>\n\n");
}

void init_data (T *data, int nx, int ny, int nz)
{
    for (int i; i < nx*ny*nx; i++)
        data[i] = 100.0;
}

void fill_send_buf (T *data, T *send_left, T *send_right, int nx, int ny, int nz)
{
    for (int k = 0; k < nz; k++) {
        for (int j = 0; j < ny; j++) {
            send_left[Index2D(j, k, ny, nz)] = data[Index3D(0, j, k, nx, ny, nz)];
            send_right[Index2D(j, k, ny, nz)] = data[Index3D(nx-1, j, k, nx, ny, nz)];
        }
    }
}

void high_level_relax (T* array, T* old_array, int nx, int ny, int nz)
{
    T *send_buf_left, *send_buf_right;
    T *recv_buf_left, *recv_buf_right;

    double start_time = MPI_Wtime();

    MPI_Alloc_mem (sizeof(T)*ny*nz, MPI_INFO_NULL, &send_buf_left);
    MPI_Alloc_mem (sizeof(T)*ny*nz, MPI_INFO_NULL, &send_buf_right);
    MPI_Alloc_mem (sizeof(T)*ny*nz, MPI_INFO_NULL, &recv_buf_left);
    MPI_Alloc_mem (sizeof(T)*ny*nz, MPI_INFO_NULL, &recv_buf_right);

    fill_send_buf (old_array, send_buf_left, send_buf_right, nx, ny, nz);

    MPI_Request pt_request_send_left, pt_request_send_right;
    MPI_Request pt_request_recv_left, pt_request_recv_right;

    if (myid > 0) {
        MPI_Isend (send_buf_left, ny*nz, MPI_T, myid-1, 0, MPI_COMM_WORLD, &pt_request_send_left);
        MPI_Irecv (recv_buf_left, ny*nz, MPI_T, myid-1, 0, MPI_COMM_WORLD, &pt_request_recv_left);
    }
    if (myid < nproc - 1) {
        MPI_Irecv (recv_buf_right, ny*nz, MPI_T, myid+1, 0, MPI_COMM_WORLD, &pt_request_recv_right);
        MPI_Isend (send_buf_right, ny*nz, MPI_T, myid+1, 0, MPI_COMM_WORLD, &pt_request_send_right);
    }

    /*
        center computation
    */
    for (int k = 1; k < nz-1; k++) {
        for (int j = 1; j < ny-1; j++) {
            for (int i = 1; i < nx-1; i++) {
                array[Index3D(i, j, k, nx, ny, nz)] =
                    (old_array[Index3D(i-1, j, k, nx, ny, nz)] +
                     old_array[Index3D(i+1, j, k, nx, ny, nz)] +
                     old_array[Index3D(i, j-1, k, nx, ny, nz)] +
                     old_array[Index3D(i, j+1, k, nx, ny, nz)] +
                     old_array[Index3D(i, j, k-1, nx, ny, nz)] +
                     old_array[Index3D(i, j, k+1, nx, ny, nz)]) / 6.0;
            }
        }
    }

    double center_time = MPI_Wtime();

    MPI_Status pt_status_send_left, pt_status_send_right;
    MPI_Status pt_status_recv_left, pt_status_recv_right;

    /*
        left boundary
    */
    if (myid > 0) {
        MPI_Wait (&pt_request_send_left, &pt_status_send_left);
        MPI_Wait (&pt_request_recv_left, &pt_status_recv_left);

        for (int k = 1; k < nz-1; k++) {
            for (int j = 1; j < ny-1; j++) {
                array[Index3D(0, j, k, nx, ny, nz)] = 
                    (recv_buf_left[Index2D(j, k, ny, nz)] +
                     old_array[Index3D(1, j, k, nx, ny, nz)] +
                     old_array[Index3D(0, j-1, k, nx, ny, nz)] +
                     old_array[Index3D(0, j+1, k, nx, ny, nz)] +
                     old_array[Index3D(0, j, k-1, nx, ny, nz)] +
                     old_array[Index3D(0, j, k+1, nx, ny, nz)]) / 6.0;
            }
        }
    }

    /*
        right boundary
    */
    if (myid < nproc - 1) {
        MPI_Wait (&pt_request_send_right, &pt_status_send_right);
        MPI_Wait (&pt_request_recv_right, &pt_status_recv_right);

        for (int k = 1; k < nz-1; k++) {
            for (int j = 1; j < ny-1; j++) {
                array[Index3D(nx-1, j, k, nx, ny, nz)] =
                    (old_array[Index3D(nx-2, j, k, nx, ny, nz)] +
                     recv_buf_right[Index2D(j, k, ny, nz)] +
                     old_array[Index3D(nx-1, j-1, k, nx, ny, nz)] +
                     old_array[Index3D(nx-1, j+1, k, nx, ny, nz)] +
                     old_array[Index3D(nx-1, j, k-1, nx, ny, nz)] +
                     old_array[Index3D(nx-1, j, k+1, nx, ny, nz)]) / 6.0;
            }
        }
    }

    MPI_Free_mem (send_buf_left);
    MPI_Free_mem (send_buf_right);
    MPI_Free_mem (recv_buf_left);
    MPI_Free_mem (recv_buf_right);

    double this_boundary_time = MPI_Wtime() - start_time;
    double this_center_time = center_time - start_time;

    MPI_Reduce (&this_center_time, &center_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce (&this_boundary_time, &boundary_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (myid == 0) {
        printf("center_time: %f\n", center_time); 
        printf("boundary_time: %f\n", boundary_time);
    }
}

int main (int argc, char *argv[])
{
    unsigned long long a, b;
    if (argc < 3) {
	    print_usage();
        return -1;
	}
	else {
		numberOfIterations = atoi(argv[1]);
		global_arraySize = atoi(argv[2]);
	}

    MPI_Init (&argc, &argv);

    MPI_Comm_rank (MPI_COMM_WORLD, &myid);
    MPI_Comm_size (MPI_COMM_WORLD, &nproc);

    int arraySize[3];

    arraySize[0] = global_arraySize;
    arraySize[1] = global_arraySize;
    arraySize[2] = global_arraySize;

    /*
        divide x dimension into #nproc processes
    */
    int singleSize = arraySize[0] / nproc;

    T *array, *old_array;

    if (myid == 0)
        printf ("Allocate array\n");

    MPI_Alloc_mem(sizeof(T)*singleSize*arraySize[1]*arraySize[2], MPI_INFO_NULL, &array);
    MPI_Alloc_mem(sizeof(T)*singleSize*arraySize[1]*arraySize[2], MPI_INFO_NULL, &old_array);

    if (myid == 0) {
        printf ("Done: Allocate array\n");
        printf ("Initialize array\n");
    }

    // Initialize the array data
    init_data(array, singleSize, arraySize[1], arraySize[2]);
    init_data(old_array, singleSize, arraySize[1], arraySize[2]);

    if (myid == 0) {
        printf ("Call Relaxation\n");
    }

    // Call relaxation on array
    double start_time = MPI_Wtime();

    for (int i = 0; i < numberOfIterations/2; i++)
    {
        high_level_relax (old_array, array, singleSize, arraySize[1], arraySize[2]);
        high_level_relax (array, old_array, singleSize, arraySize[1], arraySize[2]);
    }

    double this_time = MPI_Wtime() - start_time;

    MPI_Reduce (&this_time, &total_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (myid == 0)
        printf ("overall MPI time: %f\n", total_time);

    MPI_Free_mem (array);
    MPI_Free_mem (old_array);

    MPI_Finalize();

    return 0;
}

