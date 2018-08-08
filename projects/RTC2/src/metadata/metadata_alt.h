#ifndef _METADATA_ALT_H
#define _METADATA_ALT_H 1

#include "metalib_alt.h"

#ifdef NO_MANGLE
#ifdef __cplusplus
extern "C" {
#endif
#endif

#if TIMING_SUPPORT
extern clock_t start;
extern clock_t start_main;
extern clock_t end_main;
extern clock_t end;
#endif


extern uint64_t EnterAtFirst;
extern uint64_t EnterScope;
extern uint64_t ExitScope;
extern uint64_t UpdateInitInfo;
extern uint64_t CheckInitInfo;
extern uint64_t ExecAtLast;
extern uint64_t CreateDummyEntry;
extern uint64_t CreateEntryDest;
extern uint64_t CreateEntryLong;
extern uint64_t CreateEntry;
extern uint64_t ArrayBnd;
extern uint64_t ArrayBndLookup;
extern uint64_t CheckEntry;
extern uint64_t MallocOvl;
extern uint64_t CreateEntrySrc;
extern uint64_t ReallocOvl;
extern uint64_t NullChk;
extern uint64_t FreeOvl;
extern uint64_t FindLock;
extern uint64_t FindKey;
extern uint64_t CreateEntryBlank;
extern uint64_t ValidEntry;
extern uint64_t SpatChk;
extern uint64_t TempChk;
extern uint64_t RemoveEntry;
extern uint64_t CreateEntryLock;
extern uint64_t LowerBnd;


void v_Ret_execAtFirst();
void v_Ret_EnterScope();
void v_Ret_ExitScope();
void v_Ret_update_initinfo_L_Arg_UL_Arg(long long lock, unsigned long long addr);
void v_Ret_check_initinfo_L_Arg_UL_Arg(long long lock, unsigned long long addr);
void v_Ret_execAtLast();
void v_Ret_create_dummy_entry_UL_Arg(unsigned long long addr);
void v_Ret_create_entry_UL_Arg_UL_Arg(unsigned long long dest,unsigned long long src);
void v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(unsigned long long addr, unsigned long long base, unsigned long size, unsigned long long lock);
void v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(unsigned long long addr,unsigned long long base,unsigned long size);
#if INT_ARRAY_INDEX
void v_Ret_array_bound_check_Ui_Arg_Ui_Arg(unsigned int size, int index);
#else
void v_Ret_array_bound_check_Ui_Arg_Ui_Arg(unsigned int size, unsigned int index);
#endif /* INT_ARRAY_INDEX */
void v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(unsigned long long addr,unsigned long index);
void v_Ret_check_entry_UL_Arg_UL_Arg(unsigned long long ptr, unsigned long long addr);


struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(unsigned long size);

void v_Ret_create_entry_if_src_exists_UL_Arg_UL_Arg(unsigned long long dest, unsigned long long src);
struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_realloc_overload___Pb__v__Pe___Type_Arg_Ul_Arg(struct __Pb__v__Pe___Type str, unsigned long size);
void v_Ret_null_check_UL_Arg(unsigned long long ptr);
void v_Ret_free_overload___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input);

void _v_Ret_push_to_stack_UL_Arg(unsigned long long addr);
unsigned long long _UL_Ret_get_from_stack_i_Arg(unsigned int index);

unsigned long long _UL_Ret_pop_from_stack_i_Arg(unsigned int evict);

void v_Ret_create_entry_with_new_lock_UL_Arg_UL_Arg_Ul_Arg(unsigned long long addr,unsigned long long ptr,unsigned long size);

void v_Ret_remove_entry_UL_Arg(unsigned long long );

//
// extra auxiliary functions
// \pp \note added to work around changes in ROSE (?) mangling

void v_Ret_free_overload_class___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input);

struct __Pb__v__Pe___Type class___Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(unsigned long size);


#ifdef NO_MANGLE
#ifdef __cplusplus
}
#endif
#endif

#endif /* _METADATA_ALT_H */
