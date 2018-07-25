#include "metadata_alt.h"

#include "TrackingDB.h"


// Making this 1 ensures that the
// function names are not mangled
#ifdef NO_MANGLE
#ifdef __cplusplus
extern "C" {
#endif
#endif

// \todo \pp remove from header and use static linkage.
uint64_t EnterAtFirst = 0;
uint64_t EnterScope = 0;
uint64_t ExitScope = 0;
uint64_t UpdateInitInfo = 0;
uint64_t CheckInitInfo = 0;
uint64_t ExecAtLast = 0;
uint64_t CreateDummyEntry = 0;
uint64_t CreateEntryDest = 0;
uint64_t CreateEntryLong = 0;
uint64_t CreateEntry = 0;
uint64_t ArrayBnd = 0;
uint64_t ArrayBndLookup = 0;
uint64_t CheckEntry = 0;
uint64_t MallocOvl = 0;
uint64_t CreateEntrySrc = 0;
uint64_t ReallocOvl = 0;
uint64_t NullChk = 0;
uint64_t FreeOvl = 0;
uint64_t FindLock = 0;
uint64_t FindKey = 0;
uint64_t CreateEntryBlank = 0;
uint64_t ValidEntry = 0;
uint64_t SpatChk = 0;
uint64_t TempChk = 0;
uint64_t RemoveEntry = 0;
uint64_t CreateEntryLock = 0;
uint64_t LowerBnd = 0;

#if TIMING_SUPPORT
clock_t start;
clock_t start_main;
clock_t end_main;
clock_t end;
#endif




void v_Ret_execAtFirst() {

  #if GET_STATS
  EnterAtFirst++;
  #endif
  #if TIMING_SUPPORT
  start = clock();
  #endif

  metadata_execAtFirst();
}

void v_Ret_EnterScope() {
  #if GET_STATS
  EnterScope++;
  #endif
  metadata_EnterScope();
}



void v_Ret_ExitScope() {
  #if GET_STATS
  ExitScope++;
  #endif
  metadata_ExitScope();
}

void v_Ret_update_initinfo_L_Arg_UL_Arg(long long lock, unsigned long long addr) {
  #if GET_STATS
  UpdateInitInfo++;
  #endif
  metadata_updateInitInfo(lock, addr);
}

void v_Ret_check_initinfo_L_Arg_UL_Arg(long long lock, unsigned long long addr) {
  #if GET_STATS
  CheckInitInfo++;
  #endif
  metadata_checkInitInfo(lock, addr);
}

void v_Ret_execAtLast() {

  #if GET_STATS
  ExecAtLast++;

  printf("EnterAtFirst: %lu\n", EnterAtFirst);
  printf("EnterScope: %lu\n", EnterScope);
  printf("ExitScope: %lu\n", ExitScope);
  printf("UpdateInitInfo: %lu\n", UpdateInitInfo);
  printf("CheckInitInfo: %lu\n", CheckInitInfo);
  printf("ExecAtLast: %lu\n", ExecAtLast);
  printf("FindLock: %lu\n", FindLock);
  printf("FindKey: %lu\n", FindKey);
  printf("CreateEntryBlank: %lu\n", CreateEntryBlank);
  printf("ValidEntry: %lu\n", ValidEntry);
  printf("CreateDummyEntry: %lu\n", CreateDummyEntry);
  printf("CreateEntryDest: %lu\n", CreateEntryDest);
  printf("CrateEntryLong: %lu\n", CreateEntryLong);
  printf("CreateEntry: %lu\n", CreateEntry);
  printf("SpatChk: %lu\n", SpatChk);
  printf("TempChk: %lu\n", TempChk);
  printf("ArrayBnd: %lu\n", ArrayBnd);
  printf("ArrayBndLookup: %lu\n", ArrayBndLookup);
  printf("CheckEntry: %lu\n", CheckEntry);
  printf("RemoveEntry: %lu\n", RemoveEntry);
  printf("CreateEntryLock: %lu\n", CreateEntryLock);
  printf("MallocOvl: %lu\n", MallocOvl);
  printf("CreateEntrySrc: %lu\n", CreateEntrySrc);
  printf("LowerBnd: %lu\n", LowerBnd);
  printf("ReallocOvl: %lu\n", ReallocOvl);
  printf("NullChk: %lu\n", NullChk);
  printf("FreeOvl: %lu\n", FreeOvl);
  #endif

  printf("execAtLast: Begin\n");

  #if TIMING_SUPPORT
  end = clock();

    printf( "Total: %6.2f\n"
          , (((double) (end-start)) / CLOCKS_PER_SEC)
          );

  #endif
  metadata_execAtLast();

  printf("execAtLast: End\n");
}


void v_Ret_create_dummy_entry_UL_Arg(unsigned long long addr)
{
#if GET_STATS
  CreateDummyEntry++;
#endif

#if DEBUG
  printf("create_dummy_entry: addr: %llu\n", addr);
#endif /* DEBUG */

  metadata_create_dummy_entry(addr);
}

void v_Ret_create_entry_UL_Arg_UL_Arg(unsigned long long dest,unsigned long long src)
{
#if GET_STATS
  CreateEntryDest++;
#endif

#if DEBUG
  printf("create_entry_dest_src: dest: %llu, src: %llu\n", dest, src);
#endif /* DEBUG */

  metadata_create_entry_dest_src(dest, src);
}

void v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg_UL_Arg(unsigned long long addr, unsigned long long base, unsigned long size, unsigned long long lock)
{
#if GET_STATS
  CreateEntryLong++;
#endif

#if DEBUG
  printf("create_entry_4: addr: %llu, base: %llu, size: %lu\n", addr, base, size);
#endif /* DEBUG */

  metadata_create_entry_4(addr, base, size, lock);
}


void v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(unsigned long long addr,unsigned long long base,unsigned long size)
{
#if GET_STATS
  CreateEntry++;
#endif

#if DEBUG
  printf("create_entry_3: addr: %llu, base: %llu, size: %lu\n", addr, base, size);
#endif /* DEBUG */

  metadata_create_entry_3(addr, base, size);
}

#if INT_ARRAY_INDEX
void v_Ret_array_bound_check_Ui_Arg_Ui_Arg(unsigned int size, int index) {
#else
void v_Ret_array_bound_check_Ui_Arg_Ui_Arg(unsigned int size, unsigned int index) {
#endif /* INT_ARRAY_INDEX */
  #if GET_STATS
  ++ArrayBnd;
  #endif

  metadata_array_bound_check(size, index);
}

void v_Ret_array_bound_check_using_lookup_UL_Arg_Ul_Arg(unsigned long long addr,unsigned long index) {
  #if GET_STATS
  ++ArrayBndLookup;
  #endif

  metadata_array_bound_check_using_lookup(addr, index);
}

void v_Ret_check_entry_UL_Arg_UL_Arg(unsigned long long ptr, unsigned long long addr)
{
#if GET_STATS
  ++CheckEntry;
#endif

#if DEBUG
  printf("check_entry: ptr: %llu, addr: %llu\n", ptr, addr);
#endif /* DEBUG */

  metadata_check_entry(ptr, addr);
}

struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(unsigned long size)
{
#if GET_STATS
  MallocOvl++;
#endif

  __Pb__v__Pe___Type output;

  assert(size > 0); //RMM

  output.ptr = malloc(size);
  output.addr = reinterpret_cast<unsigned long long>(&output.ptr);
  metadata_malloc_overload(output.addr, (unsigned long long)output.ptr, size);

#if DEBUG
  printf("malloc_ovl: addr: %llu, ptr: %llu, size: %lu\n", output.addr, (unsigned long long)output.ptr, size);
#endif /* DEBUG */

  return output;
}


void v_Ret_create_entry_if_src_exists_UL_Arg_UL_Arg(unsigned long long dest, unsigned long long src)
{
#if GET_STATS
  CreateEntrySrc++;
#endif

#if DEBUG
  printf("create_entry_if_src_exists: dest: %llu, src: %llu\n", dest, src);
#endif /* DEBUG */

  metadata_create_entry_if_src_exists(dest, src);
}

__Pb__v__Pe___Type
__Pb__v__Pe___Type_Ret_realloc_overload___Pb__v__Pe___Type_Arg_Ul_Arg(struct __Pb__v__Pe___Type str, unsigned long size)
{
#if GET_STATS
  ReallocOvl++;
#endif

  assert(size > 0); //RMM

  metadata_realloc_overload_1((unsigned long long)str.ptr, str.addr);
  // Do a realloc and update the bounds.
  str.ptr = realloc(str.ptr, size);
  metadata_realloc_overload_2((unsigned long long)str.ptr, str.addr, size);
  return str;
}


void v_Ret_null_check_UL_Arg(unsigned long long ptr)
{
  #if GET_STATS
  NullChk++;
  #endif
  assert((void*)ptr != NULL);
}


void v_Ret_free_overload___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input)
{
#if GET_STATS
  FreeOvl++;
#endif

  metadata_free_overload((unsigned long long)input.ptr, input.addr);

#if DEBUG
  printf("free_ovl: ptr: %llu, addr: %llu\n", (unsigned long long)input.ptr, input.addr);
#endif /* DEBUG */

  free(input.ptr);
}

void v_Ret_push_to_stack_UL_Arg(unsigned long long addr)
{
  metadata_push_to_stack(addr);
}

unsigned long long UL_Ret_get_from_stack_i_Arg(int index)
{
  return metadata_get_from_stack(index);
}

unsigned long long UL_Ret_pop_from_stack_i_Arg(int evict)
{
  return metadata_pop_from_stack(evict);
}

void v_Ret_create_entry_with_new_lock_UL_Arg_UL_Arg_Ul_Arg(unsigned long long addr,unsigned long long ptr,unsigned long size)
{
  create_entry_with_new_lock(addr, ptr, size);
}

void v_Ret_remove_entry_UL_Arg(unsigned long long addr)
{
  remove_entry_from_addr(addr);
}

//
// extra auxiliary functions
// \pp \note added to work around changes in ROSE (?) mangling

void v_Ret_free_overload_class___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input)
{
  v_Ret_free_overload___Pb__v__Pe___Type_Arg(input);
}

struct __Pb__v__Pe___Type class___Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(unsigned long size)
{
  return __Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(size);
}


#ifdef NO_MANGLE
#ifdef __cplusplus
}
#endif
#endif

//------------------------------
