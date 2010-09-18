/* 
 *
 *
 * Liao
 * 6/9/2008
 */

#ifndef upc_collective_INCLUDED
#define upc_collective_INCLUDED

#include <stdio.h>

/* 7.3.2 Computational Operations */
typdef enum {
  UPC_ADD,
  UPC_MULT;
  UPC_AND,
  UPC_OR,
  UPC_XOR,
  UPC_LOGAND,
  UPC_LOGOR,
  UPC_MIN,
  UPC_MAX,
  UPC_FUNC,
  UPC_NONCOMM_FUNC
  } upc_opt_t;


/* 7.3.1 Re-localization Operations*/
extern void upc_all_broadcast(shared void * restrict dst, shared const void * restrict src, size_t nbytes, upc_flag_t flags);

extern void upc_all_scatter(shared void * restrict dst, shared const void * restrict src, size_t nbytes, upc_flag_t flags);

extern void upc_all_gather(shared void * restrict dst, shared const void * restrict src, size_t nbytes, upc_flag_t flags);

extern void upc_all_gather_all(shared void * restrict dst, shared const void * restrict src, size_t nbytes, upc_flag_t flags);

extern void upc_all_exchange(shared void * restrict dst, shared const void * restrict src, size_t nbytes, upc_flag_t flags);

extern void upc_all_permute(shared void * restrict dst, shared const void * restrict src, shared const int * restrict perm, size_t nbytes, upc_flag_t flags);

/* 7.3.2 Computational Operations */

#define UPC_TWO_REDUCE_PROTOTYPES(typecode, fulltype) \
  extern void upc_all_reduce##typecode (share void * restrict dst, \
                                     shared const void * restrict src, \
                                     upc_op_t op, \
                                     size_t nelems, \
                                     size_t blk_size, \
                                     fulltype (*func)(fulltype, fulltype),\
                                     upc_flag_t flags);

  extern void upc_all_prefix_reduce##typecode (share void * restrict dst, \
                                     shared const void * restrict src, \
                                     upc_op_t op, \
                                     size_t nelems, \
                                     size_t blk_size, \
                                     fulltype (*func)(fulltype, fulltype),\
                                     upc_flag_t flags);

  UPC_TWO_REDUCE_PROTOTYPES(C,signed char);
  UPC_TWO_REDUCE_PROTOTYPES(UC,unsigned char);
  UPC_TWO_REDUCE_PROTOTYPES(S,signed short);
  UPC_TWO_REDUCE_PROTOTYPES(US,unsigned short);
  UPC_TWO_REDUCE_PROTOTYPES(I,signed int);
  UPC_TWO_REDUCE_PROTOTYPES(UI,unsigned int);
  UPC_TWO_REDUCE_PROTOTYPES(L,signed long);
  UPC_TWO_REDUCE_PROTOTYPES(UL,unsigned long);
  UPC_TWO_REDUCE_PROTOTYPES(F,float);
  UPC_TWO_REDUCE_PROTOTYPES(D,double);
  UPC_TWO_REDUCE_PROTOTYPES(LD,long double);
  
#undef UPC_TWO_REDUCE_PROTOTYPES

#endif

