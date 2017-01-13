#ifndef _bsparti_h
#define _bsparti_h

#ifdef HAVE_CONFIG_H
/* #include <config.h> */
#endif

#if 0
#ifdef USE_MPI
#define PARTI_ENABLE_MP_INTERFACE_MPI
/* error "MPI defined in bsparti.h" */
#else
#error "MPI NOT defined in bsparti.h"
#endif
#endif
#include <Parti_config.h>

/* Set PARTI debugging based on the use of INTERNALDEBUG in P++ */
#if defined(INTERNALDEBUG)
#define PARTI_INTERNALDEBUG 1
#endif

#if 0
/* Modified by Dan Quinlan: removed since these are not used */
/* #ifdef __STDC__ */
#ifdef STD_COMPLIANT_COMPILER
#  include <stdarg.h>
#  define VA_DCL(type,var)              (type var,...)
#  define VA_START(list,var,type)       ((va_start(list,var)) , (var))
#else
#  include <varargs.h>
#  define VA_DCL(type,var)               (va_alist) va_dcl
#  define VA_START(list,var,type)        ((va_start(list)), va_arg(list,type))
#endif
#endif

/* #ifdef __STDC__ */
#ifdef STD_COMPLIANT_COMPILER
# define P_(a)	a
#else
# define P_(a)  ()
#endif /* __STDC__ */

#define fMIN(A, B)  ((A) < (B) ? (A) : (B))
#define fMAX(A, B)  ((A) > (B) ? (A) : (B))
#define NEW(A)     (A *) malloc(sizeof(A))
#define NEWN(A, N) (A *) malloc(sizeof(A) * (unsigned)(N))
#define DISPOSE(A) free(A)
#define PARTI_ASSERT(a, s) 

/* Modified by Dan Quinlan (bugfix (12/19/96) libraries should not modify the system NULL macro; bad style)
   define NULL              0 
*/
#define TRUE              1
#define FALSE             0
#define INT_SZ  sizeof(int)
#define FLT_SZ sizeof(float)
#define DBL_SZ sizeof(double)
#define CHR_SZ sizeof(char)

/*
 Modified by Dan Quinlan 5/1/2000 to increase the number of processors. 
 define MAX_NODES       128
 */
#define MAX_NODES      1024
#define MAX_NMSG        500
#define MAX_MSG_SZ   256000
#define NODE_PID          0
#define HOST_PID          1
#define ALL_NODES        -1
#define ALL_PIDS         -1

#define HTYPE1	         0 
#define HTYPE2	         40 
#define NTYPE1	         100 
#define NTYPE2	         140 
#define T_TYPE	         300 
#define  MAX_DIM         6





typedef struct schedData_rec {
  int proc ;    /* Processor Number   */
  int numDims ; /* No. of dimensions in the arrays involved       */
  int  startPosn ; /* Starting position of the very first element */

  int  numelem[MAX_DIM] ; 
  int  str[MAX_DIM] ; /* Stride in each dimension includes the    */ 
                      /* multiplier factor                        */  
 } SchedData; 




typedef struct nsched {
   int             rMsgSz[MAX_NODES], sMsgSz[MAX_NODES];
   SchedData *rData[MAX_NODES];
   SchedData *sData[MAX_NODES];
   int type;  /* 1 for exch_sched, 2 for subarray_sched */
   int hash;  /* bucket in the hash table */
   int referenceCount;  /* Added by Dan Quinlan */
   int ID_Number;  /* Added by Dan Quinlan */
}SCHED;




/* describes a range of ghost cells to fill, or a block to move */
/* Rewritten : Gagan 07/05/93 */ 
struct range_rec {
   int             rangeDims[MAX_DIM]; 
   int             max[MAX_DIM]; 
   int             min[MAX_DIM] ; 
   int             step[MAX_DIM] ;
   int             indexVals[MAX_DIM];  /* the indices for each array dimension */
};

/*  Rewritten: Gagan 07/05/93 */
struct dest {
   char            name[10];
   int             *ptr;
   int             startPosn ; 
   int             step[MAX_DIM] ;
};


typedef struct dArray_rec {
   int             nDims;
   int             *ghostCells;   /* number of internal ghost cells in each dim */
   int             *dimVecG;      /* total size of each dim */
   int             *dimVecL;      /* local size of each dim for central pieces*/
/* Gupta 5/22 */
   int             *dimVecL_L;    /* local size of each dim for left most piece */
   int             *dimVecL_R;    /* local size of each dim for right most piece */
   int             *g_index_low;  /* lower global index on my processor */
   int             *g_index_hi;   /* upper global index on my processor */
   int             *local_size;   /* Local size on my processor */

/**************/
   int             *decompDim;    /* dim of decomp to which each dim aligned 
                                   * defines how array aligned to decomp
                                   * used with decomp to inialize decompPosn
                                   * and dimDist 
                                   */
   int             *decompPosn;   /* coordinate position of processor in 
                                     the decomposition to which it's bound */
                                  /* in the multi-dimensional decomposition
                                     space */
   char            *dimDist;      /* type of distribution in each dim */
   struct decomp_rec *decomp;      /* decomposition to which processor bound */
   int referenceCount;  /* Added by Dan Quinlan */
} DARRAY;

typedef struct decomp_rec {
   int             nDims, nProcs, baseProc;
   int             *dimVec;       /* size of decomposition in each dim */
   int             *dimProc;      /* num processors allocated to each dim */
   char            *dimDist;      /* type of distribution in each dim */
   int referenceCount;  /* Added by Dan Quinlan */
} DECOMP;


typedef struct vProc_rec {
   int             nDims, nProcs, usedProcs;
   int             *dimVec;
} VPROC;

struct range {
   int             rangeDim, max, min, stride;
};

struct exch_rec {
   /* for a fixed index */
   int             nFixedIndices;
   int             indexDim[MAX_DIM], indexVals[MAX_DIM];
   /* for a range of indices */
   int             numRanges, rangeDim[MAX_DIM];
   struct range    ranges[MAX_DIM];
};

struct time_rec {
   double            total, comp, comm;
};

/* Use the more complete prototypes for P++ (required for use with C++) */
#if 0
SCHED *exchSched(), *ghostFillSched();
SCHED *ghostFillSpanSched(), *ghostFillAllSched();
SCHED *subArraySched();
void iDataMove(), fDataMove(), dDataMove(), cDataMove(); 
DECOMP *create_decomp();
void distribute();
void embed();
int lalbnd(), laubnd();
DARRAY *align();
VPROC *vProc();
void laSizes();
int gLBnd(), gUBnd();
int localToGlobal(), globalToLocal();
int localToGlobalWithGhost(), globalToLocalWithGhost();
void remove_exch_scheds();
void remove_subarray_scheds();
void free_sched();
#else
SCHED *exchSched(DARRAY*,int,int);
SCHED *ghostFillSched(DARRAY*,int,int*);
SCHED *ghostFillSpanSched(DARRAY*,int,int*);
SCHED *ghostFillAllSched(DARRAY*);
SCHED *subArraySched(DARRAY*,DARRAY*,int,int*,int*,int*,int*,int*,int*,int*,int*);
void iDataMove(int*,SCHED*,int*);
void fDataMove(float*,SCHED*,float*);
void dDataMove(double*,SCHED*,double*);
void cDataMove(char*,SCHED*,char*);
DECOMP *create_decomp(int,int*);
void distribute(DECOMP*,char*);
void embed(DECOMP*,VPROC*,int,int);
int lalbnd(DARRAY*,int,int,int);
int laubnd(DARRAY*,int,int,int);
DARRAY *align(DECOMP*,int,int*,int*,int*,int*,int*,int*,int*);
VPROC *vProc(int,int*);
void laSizes(DARRAY*,int*);
int gLBnd(DARRAY*,int);
int gUBnd(DARRAY*,int);
int localToGlobal(DARRAY*,int,int);
int globalToLocal(DARRAY*,int,int);
int localToGlobalWithGhost(DARRAY*,int,int);
int globalToLocalWithGhost(DARRAY*,int,int);
void remove_exch_scheds();
void remove_subarray_scheds();
void free_sched(SCHED*);
void init_Sched(SCHED*);
void merge_sched(SCHED*,SCHED*);
int get_proc_num (DARRAY*,int*);

/* Function added by Dan Quinlan at Los Alamos National Labs 
   it frees memory after using PARTI as a means of tracing
   memory leaks in software using PARTI
 */
void cleanup_after_PARTI ();
void delete_DARRAY ( DARRAY* X );
void delete_SCHED  ( SCHED*  X );
void delete_DECOMP ( DECOMP* X );
void delete_VPROC  ( VPROC*  X );

/* Message passing prototypes */
#if defined(PARTI_ENABLE_MP_INTERFACE_PVM)
#include <pvm3.h>
int jimsend(int tag,char* buf,int len,int node,int pid);
int jimrecv(int tag,char* buf,int len,int node,int pid);
#endif

#if defined(PARTI_ENABLE_MP_INTERFACE_MPI)
#include <mpi.h>
#include "mpi-port.h"
#endif

/* Include Parti Macros */
#include "port.h"
#endif

/* globals */
extern int FFlag;

/* Specified as part of P++ diagnostics */
extern int PARTI_numberOfMessagesSent;
extern int PARTI_numberOfMessagesRecieved;
extern int PARTI_messagePassingInterpretationReport;

#endif /* _bsparti_h */
