/* DQ (2/3/2011): Bug report from iastate: arg_with_THREADS_reparse/many_cases.upc */

#include <upc.h>
#include <upc_collective.h>
#include <upc_io.h>

#define NELEMS 10
#define BLK_SIZE 3
#define IO_FILE_NAME "upc_check_test_ip_op_file.upc"

shared int *scatter_A;
shared [] int *gather_B;
shared [BLK_SIZE] signed char reduce_C_A[NELEMS*THREADS];
shared signed char reduce_C_B;
shared [BLK_SIZE] unsigned char reduce_UC_A[NELEMS*THREADS];
shared unsigned char reduce_UC_B;
shared [BLK_SIZE] short reduce_S_A[NELEMS*THREADS];
shared short reduce_S_B;
shared [BLK_SIZE] unsigned short reduce_US_A[NELEMS*THREADS];
shared unsigned short reduce_US_B;
shared [BLK_SIZE] int reduce_I_A[NELEMS*THREADS];
shared int reduce_I_B;
shared [BLK_SIZE] unsigned int reduce_UI_A[NELEMS*THREADS];
shared unsigned int reduce_UI_B;
shared [BLK_SIZE] long reduce_L_A[NELEMS*THREADS];
shared long reduce_L_B;
shared [BLK_SIZE] unsigned long reduce_UL_A[NELEMS*THREADS];
shared unsigned long reduce_UL_B;
shared [BLK_SIZE] float reduce_F_A[NELEMS*THREADS];
shared float reduce_F_B;
shared [BLK_SIZE] double reduce_D_A[NELEMS*THREADS];
shared double reduce_D_B;
shared [BLK_SIZE] long double reduce_LD_A[NELEMS*THREADS];
shared long double reduce_LD_B;

shared [5] char buffer[10*THREADS];
shared [8] char bufferA[8*THREADS];

int main ()
{
   upc_off_t nbytes;
   upc_file_t *fd;
   struct upc_filevec filevec[2];
   struct upc_local_memvec l_memvec[2];
   struct upc_shared_memvec s_memvec[2];
   size_t af_size;
   char local_buffer[10];
   char file_name[100] = IO_FILE_NAME;

   l_memvec[0].baseaddr=local_buffer;
   l_memvec[0].len=4*sizeof(char);
   l_memvec[1].baseaddr=&local_buffer[7];
   l_memvec[1].len=3*sizeof(char);

   s_memvec[0].baseaddr=&bufferA[0+MYTHREAD*8];
   s_memvec[0].blocksize=8*sizeof(char);
   s_memvec[0].len=4*sizeof(char);
   s_memvec[1].baseaddr=&bufferA[5+MYTHREAD*8];
   s_memvec[1].blocksize=8*sizeof(char);
   s_memvec[1].len=3*sizeof(char);


   filevec[0].offset=(MYTHREAD*10)*sizeof(char);
   filevec[0].len=2*sizeof(char);
   filevec[1].offset=(5+MYTHREAD*10)*sizeof(char);
   filevec[1].len=5*sizeof(char);

/**************************************************************/
/* Tests start, one line for each test                        */
/**************************************************************/         
   /* Example of upc_all_alloc() */
   scatter_A = (shared int *)upc_all_alloc(THREADS, THREADS*NELEMS*sizeof(int));
  
   gather_B = (shared [] int *) upc_all_alloc(1,NELEMS*THREADS*sizeof(int));
   
   /* Example of upc_affinitysize() */
   af_size = upc_affinitysize(THREADS*5*NELEMS, NELEMS, 0);

   /* Example of upc_all_reduceC */
   upc_all_reduceC(&reduce_C_B, reduce_C_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_reduceUC */
   upc_all_reduceUC(&reduce_UC_B, reduce_UC_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_reduceS */
   upc_all_reduceS(&reduce_S_B, reduce_S_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_reduceUS */
   upc_all_reduceUS(&reduce_US_B, reduce_US_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_reduceI */
   upc_all_reduceI(&reduce_I_B, reduce_I_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_reduceUI */
   upc_all_reduceUI(&reduce_UI_B, reduce_UI_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_reduceL */
   upc_all_reduceL(&reduce_L_B, reduce_L_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_reduceUL */
   upc_all_reduceUL(&reduce_UL_B, reduce_UL_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_reduceF */
   upc_all_reduceF(&reduce_F_B, reduce_F_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_reduceD */
   upc_all_reduceD(&reduce_D_B, reduce_D_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_reduceLD */
   upc_all_reduceLD(&reduce_LD_B, reduce_LD_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_prefix_reduceC */
   upc_all_prefix_reduceC(&reduce_C_B, reduce_C_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_prefix_reduceUC */
   upc_all_prefix_reduceUC(&reduce_UC_B, reduce_UC_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_prefix_reduceS */
   upc_all_prefix_reduceS(&reduce_S_B, reduce_S_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_prefix_reduceUS */
   upc_all_prefix_reduceUS(&reduce_US_B, reduce_US_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_prefix_reduceI */
   upc_all_prefix_reduceI(&reduce_I_B, reduce_I_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_prefix_reduceUI */
   upc_all_prefix_reduceUI(&reduce_UI_B, reduce_UI_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_prefix_reduceL */
   upc_all_prefix_reduceL(&reduce_L_B, reduce_L_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_prefix_reduceUL */
   upc_all_prefix_reduceUL(&reduce_UL_B, reduce_UL_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_prefix_reduceF */
   upc_all_prefix_reduceF(&reduce_F_B, reduce_F_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_prefix_reduceD */
   upc_all_prefix_reduceD(&reduce_D_B, reduce_D_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   /* Example of upc_all_prefix_reduceLD */
   upc_all_prefix_reduceLD(&reduce_LD_B, reduce_LD_A, UPC_ADD, NELEMS*THREADS, BLK_SIZE, NULL, UPC_IN_NOSYNC | UPC_OUT_NOSYNC );

   //Not a test line
   fd = upc_all_fopen(((IO_FILE_NAME)),UPC_COMMON_FP|UPC_RDWR|UPC_CREATE, 0, NULL);

   /* Example of upc_all_fseek */
   upc_all_fseek(fd, 10*MYTHREAD, UPC_SEEK_SET);

   /* Example of upc_all_fpreallocate */
   nbytes = upc_all_fpreallocate(fd,10*THREADS);

   /* Example of  upc_all_fread_shared() */
   nbytes = upc_all_fread_shared(fd, buffer, upc_blocksizeof(buffer), sizeof(char), 5*THREADS, UPC_IN_ALLSYNC | UPC_OUT_ALLSYNC);

   /* Example of  upc_all_fwrite_shared() */
   nbytes = upc_all_fwrite_shared(fd, buffer, upc_blocksizeof(buffer), sizeof(char), 5*THREADS, UPC_IN_ALLSYNC | UPC_OUT_ALLSYNC);

   /* Example of upc_all_fread_shared_async () */
   upc_all_fread_shared_async(fd, buffer, upc_blocksizeof(buffer), sizeof(char), 5*THREADS, UPC_IN_ALLSYNC | UPC_OUT_ALLSYNC);

   /* Example of upc_all_fwrite_shared_async () */
   upc_all_fwrite_shared_async(fd, buffer, upc_blocksizeof(buffer), sizeof(char), 5*THREADS, UPC_IN_ALLSYNC | UPC_OUT_ALLSYNC);

   // Not a test line 
   upc_all_fclose(fd);
  
   return 0;
}
