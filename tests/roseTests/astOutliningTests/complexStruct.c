// A test case from a changed version of smg2000 kernel
// for recursive copy dependent declarations and typedef used in an outlining target
// to a new file.
// Liao, 5/8/2009
typedef int MYINT;
typedef MYINT hypre_MPI_Comm; // test chain of typedef here

typedef int hypre_MPI_Datatype;

typedef int hypre_Index[3];

// test a defining typedef with embedded struct definition
typedef struct hypre_Box_struct 
{
   hypre_Index imin;
   hypre_Index imax;

} hypre_Box;

typedef struct hypre_BoxArray_struct //5
{
   hypre_Box *boxes;
   int size;
   int alloc_size;

} hypre_BoxArray;

typedef struct hypre_RankLink_struct
{
   int rank;
   struct hypre_RankLink_struct *next;

} hypre_RankLink;

// test base type of typedef: pointer to a typedef, then a struct
typedef hypre_RankLink *hypre_RankLinkArray[3][3][3]; 

typedef struct hypre_BoxNeighbors_struct
{
   hypre_BoxArray *boxes;
   int *procs;
   int *ids;
   int first_local;
   int num_local;
   int num_periodic;

   hypre_RankLinkArray *rank_links;

} hypre_BoxNeighbors;


typedef struct hypre_StructGrid_struct
{
   hypre_MPI_Comm comm;

   int dim;

   hypre_BoxArray *boxes;
   int *ids;

   hypre_BoxNeighbors *neighbors;
   int max_distance;

   hypre_Box *bounding_box;

   int local_size;
   int global_size;

   hypre_Index periodic;

   int ref_count;

} hypre_StructGrid;

typedef struct hypre_StructStencil_struct  //10 
{
   hypre_Index *shape;
   int size;
   int max_offset;

   int dim;

   int ref_count;

} hypre_StructStencil;

typedef struct hypre_CommTypeEntry_struct
{
   hypre_Index imin;
   hypre_Index imax;
   int offset;

   int dim;
   int length_array[4];
   int stride_array[4];

} hypre_CommTypeEntry;


typedef struct hypre_CommType_struct
{
   hypre_CommTypeEntry **comm_entries;
   int num_entries;

} hypre_CommType;

typedef struct hypre_CommPkg_struct
{
   int num_values;
   hypre_MPI_Comm comm;

   int num_sends;
   int num_recvs;
   int *send_procs;
   int *recv_procs;


   hypre_CommType **send_types;
   hypre_CommType **recv_types;
   hypre_MPI_Datatype *send_mpi_types;
   hypre_MPI_Datatype *recv_mpi_types;


   hypre_CommType *copy_from_type;
   hypre_CommType *copy_to_type;

} hypre_CommPkg;


typedef struct hypre_StructMatrix_struct //14
{
  hypre_MPI_Comm comm;

  hypre_StructGrid *grid;
  hypre_StructStencil *user_stencil;
  hypre_StructStencil *stencil;
  int num_values;

  hypre_BoxArray *data_space;

  double *data;
  int data_alloced;
  int data_size;
  int **data_indices;

  int symmetric;
  int *symm_elements;
  int num_ghost[6];

  int global_size;

  hypre_CommPkg *comm_pkg;

  int ref_count;

} hypre_StructMatrix;

void OUT__1__6119__(void **__out_argv)
{
  hypre_StructMatrix *A =  *((hypre_StructMatrix **)(__out_argv[20]));
  int ri =  *((int *)(__out_argv[19]));
  double *rp =  *((double **)(__out_argv[18]));
  int stencil_size =  *((int *)(__out_argv[17]));
  int i =  *((int *)(__out_argv[16]));
  int (*dxp_s)[15UL] = (int (*)[15UL])(__out_argv[15]);
  int hypre__sy1 =  *((int *)(__out_argv[14]));
  int hypre__sz1 =  *((int *)(__out_argv[13]));
  int hypre__sy2 =  *((int *)(__out_argv[12]));
  int hypre__sz2 =  *((int *)(__out_argv[11]));
  int hypre__sy3 =  *((int *)(__out_argv[10]));
  int hypre__sz3 =  *((int *)(__out_argv[9]));
  int hypre__mx =  *((int *)(__out_argv[8]));
  int hypre__my =  *((int *)(__out_argv[7]));
  int hypre__mz =  *((int *)(__out_argv[6]));
  int si =  *((int *)(__out_argv[5]));
  int ii =  *((int *)(__out_argv[4]));
  int jj =  *((int *)(__out_argv[3]));
  int kk =  *((int *)(__out_argv[2]));
  const double *Ap_0 =  *((const double **)(__out_argv[1]));
  const double *xp_0 =  *((const double **)(__out_argv[0]));
// real kernel which should be outlined
  for (si = 0; si < stencil_size; si++) 
    for (kk = 0; kk < hypre__mz; kk++) 
      for (jj = 0; jj < hypre__my; jj++) 
        for (ii = 0; ii < hypre__mx; ii++) 
          rp[(ri + ii) + (jj * hypre__sy3) + (kk * hypre__sz3)] -= Ap_0[ii + (jj * hypre__sy1) + (kk * hypre__sz1) + A -> data_indices[i][si]] * xp_0[ii + (jj * hypre__sy2) + (kk * hypre__sz2) + ( *dxp_s)[si]];
}

