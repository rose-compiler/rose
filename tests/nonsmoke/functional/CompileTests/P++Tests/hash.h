#ifndef _hash_h
#define _hash_h

#include "bsparti.h"

extern int insert_da_table();
extern DARRAY* lookupda();

extern void insert_exch_table();
extern void delete_exch_table();
extern SCHED* lookup_exch();
extern void destroy_exch_table();

extern void insert_sub_table();
extern void delete_sub_table();
extern SCHED* lookup_sub();
extern void destroy_sub_table();

#endif /* _hash_h */
