#include "libxomp_mpi.h"
#include <mpi.h> // avoid polluting global headers
#include <assert.h>

int xomp_init_mpi (int* argcp, char** argvp[], int * rank, int * nprocs)
{
  int rt = 0;
  //TODO add error code handling
  MPI_Init (argcp, argvp);
  MPI_Comm_rank (MPI_COMM_WORLD, rank);
  MPI_Comm_size (MPI_COMM_WORLD, nprocs);

  return rt; 
}

// simplest range dividing function: a range of elements/iterations, divided by Count threads/processes, what is the range for thread/process ID
void xomp_static_even_divide_lower_upper (int start, int end, int thread_count, int thread_id, int* n_lower, int* n_upper)
{
  int n = end - start + 1; // inclusive end bound, 0 to 9
  int size = n / thread_count;
  int offset = size * thread_id;
  int remainder = n% thread_count;
  if(thread_id < remainder)
  {
    size++;
  }

  if(thread_id >= remainder)
    offset += remainder;  // all extra elements on top of base line offset size*tid
  else
    offset += thread_id;   // only partial extra elements   

  *n_lower = offset;
  *n_upper = offset +size -1;
}

void xomp_static_even_divide_start_size (int orig_start, int orig_size, int thread_count, int thread_id, int* n_lower, int* n_size)
{
  int size = orig_size / thread_count;
  int offset = size * thread_id;
  int remainder = orig_size % thread_count;
  if(thread_id < remainder)
  {
    size++;
  }

  if(thread_id >= remainder)
    offset += remainder;  // all extra elements on top of base line offset size*tid
  else
    offset += thread_id;   // only partial extra elements   

  *n_lower = offset;
//  *n_upper = offset +size -1;
  *n_size = size; 

}

// A helper function to obtain type size
/*
Element type ID: 
ID  MPI Datatype    C Type
  ----------------------------------------------
0  MPI_CHAR        signed char
1  MPI_SHORT       signed short i
2  MPI_INT         signed int
3  MPI_LONG        signed long int

4  MPI_UNSIGNED_CHAR       unsigned char
5  MPI_UNSIGNED_SHORT      unsigned short int
6  MPI_UNSIGNED    unsigned int
7  MPI_UNSIGNED_LONG       unsigned long int

8  MPI_FLOAT       float
9  MPI_DOUBLE      double
10  MPI_LONG_DOUBLE         long double
11  MPI_BYTE        (none) 
12  MPI_PACKED      (none)             
*/
static int getMPITypeInfo(int tid, MPI_Datatype* dtype)
{
  int rt=0;

  if (tid == 0)
  {
    rt = sizeof(char );
    *dtype = MPI_CHAR; 
  }
  else if (tid == 1)
  {
    rt = sizeof(short);
    *dtype = MPI_SHORT; 
  }
  else if (tid == 2)
  {
    rt = sizeof(int);
    *dtype = MPI_INT; 
  }
  else if (tid == 3)
  {
    rt = sizeof(long int);
    *dtype = MPI_LONG; 
  }
  else if (tid == 4)
  {
    rt = sizeof(unsigned char);
    *dtype = MPI_UNSIGNED_CHAR; 
  }
  else if (tid == 5)
  {
    rt = sizeof(unsigned short int);
    *dtype = MPI_UNSIGNED_SHORT; 
  }
  else if (tid == 6)
  {
    rt = sizeof(unsigned int);
    *dtype = MPI_UNSIGNED; 
  }
  else if (tid == 7)
  {
    rt = sizeof(unsigned long int);
    *dtype = MPI_UNSIGNED_LONG; 
  }
  else if (tid == 8)
  {
    rt = sizeof(float);
    *dtype = MPI_FLOAT; 
  }
  else if (tid == 9)
  {
    rt = sizeof(double );
    *dtype = MPI_DOUBLE; 
  }
  else if (tid == 10)
  {
    rt = sizeof(long double);
    *dtype = MPI_LONG_DOUBLE; 
  }
  else 
  {
    printf ("xomp_mpi.c: getTypeSize(tid) encounters unsupported tid %d\n", tid);
    assert (false);
  }

  return  rt ;
}

/* spread a source buffer to multiple smaller buffers in all processes , store the buffer into distsrc*/
void xomp_divide_scatter_array_to_all (double* sourceDataPointer, int x_dim_size, int y_dim_size, int z_dim_size, 
 int distributed_dimension_id, int halo_size, int rank_id, int process_count, double** distsrc)
{
  int offsetdest;
  int distdestsize;
  int nprocs = process_count; 

  // the original size info. without halo regions
  int base_x_size = x_dim_size - 2* halo_size;  
  int base_y_size  = y_dim_size -2 * halo_size;
  int base_z_size = z_dim_size -2 * halo_size;

  //TODO: currently only splitting on the 3rd dimension is implemented. 
  assert (distributed_dimension_id == 2); // dim id starts from 0, 1 to 2.
  xomp_static_even_divide_start_size (0, base_z_size, nprocs, rank_id, & offsetdest, & distdestsize);

  // TODO support other MPI types
  int elementsize =sizeof(double);
  MPI_Datatype mpitype = MPI_DOUBLE; 

//  elementsize = getMPITypeInfo(element_type_id, &mpitype);
  if(rank_id == 0)
  {     
    //  [starting offset: size]                                                                                                  
    //  This is already calculated by the data distribution runtime function?                                                    
    //  Not really, halo region is not counted.                                                                                  
    int copyOffset = offsetdest * x_dim_size * y_dim_size;                                                               
    int copySize = (distdestsize + 2 * halo_size) * x_dim_size * y_dim_size;                                                
    assert (copyOffset == offsetdest);                                                                                           
    assert (copyOffset == 0); // master process always starts from offset 0                                                      

    // local copy (this is optional, but simpler for transformation)                                                             
    memcpy(*distsrc, sourceDataPointer+copyOffset, copySize* elementsize );                                                        

    // team leader send source data to all members                                                                               
    if( nprocs > 1)
    {
      int dest, send_tag=1;                                                                                                      
      MPI_Request send_reqs[nprocs-1];                                                                                           
      MPI_Status send_status[nprocs-1];                                                                                          
      // dest rank                                                                                                               
      for(dest = 1; dest < nprocs; ++dest)                                                                                       
      {                                                                                                                          
        // calculate offset:size for each slave process's source data                                                             
        // TODO SUPPORT other dimensions
        // TODO using a runtime call to split it 
        int sendSize = base_z_size/ nprocs;                                                                                
        int sendOffset = dest * sendSize;                                                                                        
        if(dest < base_z_size%nprocs)                                                                                       
        {                                                                                                                        
          sendSize++;                                                                                                            
        }                                                                                                                        
        sendSize = (sendSize+2)* x_dim_size * y_dim_size;                                                                 
        if(dest >= base_z_size% nprocs)                                                                                      
          sendOffset += base_z_size%nprocs;                                                                                 
        else                                                                                                                     
          sendOffset += dest;                                                                                                    
        sendOffset = sendOffset* x_dim_size * y_dim_size;                                                                 
#if debug                                                                                                                        
        cout << "Master send size " << sendSize<< " from offset " << sendOffset << " "  << " to " << dest << endl;                       
#endif                                                                                                                           
        MPI_Isend(sourceDataPointer+sendOffset, sendSize, mpitype, dest, send_tag, MPI_COMM_WORLD,&send_reqs[dest-1]);         
        //     int idx;                                                                                                                  
        //     for(idx = 0; idx < sendSize; ++idx)                                                                                       
        //      printf("Source send to dest:%d result %d: %f\n",dest, idx, sourceDataPointer[offsetsrc+sendOffset+idx]);                 
      }                                                                                                                          
      MPI_Waitall(nprocs-1,send_reqs,send_status);                                                                               
    }                                                                                                                            
  }                                                                                                                              
  else                                                                                                                           
  {                                                                                                                              
    //cout << "Rank dest: " << rank << " " << offsetdest << " " << distdestsize << " " << arraySize_Z_dest << endl;                  
    // receive data from master                                                                                                      
    int src = 0, recv_tag=1;                                                                                                     
    MPI_Request recv_reqs[1];                                                                                                    
    MPI_Status recv_status[1];                                                                                                   

    int recvSize = (distdestsize + 2 * halo_size) * x_dim_size * y_dim_size;                                                
    // receiving data, the receiving buffer will be modified, must pass by reference/address here
    MPI_Irecv(*distsrc, recvSize, mpitype, src, recv_tag, MPI_COMM_WORLD, &recv_reqs[0]);                                      
    //  MPI_Irecv(distdest, distsrcsize*bxsrc.size(0)*bxsrc.size(1), MPI_DOUBLE, src, tag, MPI_COMM_WORLD, &reqs[1]);                
    MPI_Waitall(1,recv_reqs,recv_status);                                                                                        
    //  int idx;                                                                                                                     
    //  for(idx = 0; idx < distsrcsize*bxsrc.size(0)*bxsrc.size(1); ++idx)                                                           
    //   printf("Receiver:%d has result %d: %f\n",rank, idx, distsrc[idx]);                                                          
  }   

}

//  scattered array portions indicated by *distdest
//  need to be collected to destnationDataPointer
// TODO: support the case with non-zero halo size!!
//TODO support MPI data types other than MPI_DOUBLE
void xomp_collect_scattered_array_from_all (double* distdest, int x_dim_size, int y_dim_size, int z_dim_size,
 int distributed_dimension_id, int halo_size, int rank_id, int process_count, double** destinationDataPointer )
{
  int rank = rank_id;
  int nprocs = process_count;
  int offsetdest, distdestsize; 

// TODO: support the case with non-zero halo size!!
  int arraySize_X_dest = x_dim_size;
  int arraySize_Y_dest = y_dim_size;
  int arraySize_Z_dest = z_dim_size;

  xomp_static_even_divide_start_size (0, arraySize_Z_dest, nprocs, rank_id, & offsetdest, & distdestsize);

  if(rank == 0)
  {
    // team leader receives destination data from all members
    int src, recv_tag=1;
    MPI_Request recv_reqs[nprocs-1];
    MPI_Status recv_status[nprocs-1];
    for(src = 1; src < nprocs; ++src)
    { // partitions of the result (destination) array 
      int recvSize = arraySize_Z_dest / nprocs;
      int recvOffset = src * recvSize;
      if(src < arraySize_Z_dest%nprocs)
      { 
        recvSize++;
      }
      recvSize *= arraySize_X_dest*arraySize_Y_dest;
      if(src >= arraySize_Z_dest%nprocs)
        recvOffset += arraySize_Z_dest%nprocs;
      else
        recvOffset += src;
      recvOffset = recvOffset*arraySize_X_dest*arraySize_Y_dest;
      // TODO: careful with dereferencing a pointer
      MPI_Irecv(*destinationDataPointer+recvOffset, recvSize, MPI_DOUBLE, src, recv_tag, MPI_COMM_WORLD,&recv_reqs[src-1]);
    }
    MPI_Waitall(nprocs-1,recv_reqs,recv_status);
    // local copy (this could be optional, but simplier for transformation)
    //memcpy(destinationDataPointer+offsetdest, distdest,distdestsize* bxdest.size(0)* bxdest.size(1)*sizeof(double));
    memcpy(*destinationDataPointer+offsetdest, distdest,distdestsize* arraySize_X_dest * arraySize_Y_dest *sizeof(double));

    //end = MPI_Wtime();
    //elapsed_secs = (end - begin);
    //cout << "Exec. time for MPI code: " << elapsed_secs << endl;

  }
  else
  {
    int dest = 0, send_tag=1;
    MPI_Request send_reqs[1];
    MPI_Status send_status[1];
    // team members send back data
    MPI_Isend(distdest, distdestsize*arraySize_X_dest*arraySize_Y_dest, MPI_DOUBLE, dest, send_tag, MPI_COMM_WORLD, send_reqs);
    //  int idx;
    //  for(idx = 0; idx < distdestsize*arraySize_X_dest*arraySize_Y_dest; ++idx)
    //   printf("rank %d send result %d: %f\n",rank, idx, distdest[idx]); 
    MPI_Waitall(1,send_reqs,send_status);
  }

}

