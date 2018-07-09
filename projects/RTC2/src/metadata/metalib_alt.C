#include <iostream>

#include "metalib_alt.h"

#include "TrackingDB.h"

#ifdef NO_MANGLE
#ifdef __cplusplus
extern "C" {
#endif
#endif


#if THREADX_DEBUG
std::ofstream threadx_file; // \pp moved definition to C file
#endif /* THREADX_DEBUG */

#if SILENT_ERRORS
std::ofstream error_file; // \pp moved definition to C file
#endif /* SILENT_ERRORS */


LockMgr LockDB;

MetaDataMgr TrackingDB;

std::vector<unsigned long long> argMetadata;

extern uint64_t FindLock;
extern uint64_t FindKey;
extern uint64_t CreateEntryBlank;
extern uint64_t ValidEntry;
extern uint64_t SpatChk;
extern uint64_t TempChk;
extern uint64_t RemoveEntry;
extern uint64_t CreateEntryLock;
extern uint64_t LowerBnd;

/*
uint64_t getDummyLock() {
  return DUMMY_LOCK;
}

uint64_t getDummyKey() {
  return DUMMY_KEY;
}
*/

uint64_t find_lock(unsigned long long /*base*/) {
  // Some number
  return 0;
}

uint64_t find_key(uint64_t /*lock*/) {
  // Some number
  return 0;
}

void create_blank_entry(unsigned long long addr) {
  TrackingDB.create_blank_entry(addr);
}

bool isValidEntry(unsigned long long addr) {
  // was: return TrackingDB.isValidEntry(addr);
  TrackingDB.isValidEntry(addr);
  return true;
}

void spatial_check(unsigned long long ptr, unsigned long long lower, unsigned long long upper) {

#if USE_DUMMY_BOUNDS
  if (lower == DUMMY_LOWER_BOUND && upper == DUMMY_UPPER_BOUND) {
    return;
  }
#endif /* USE_DUMMY_BOUNDS */

  // More descriptive than just assert
#if DESCRIPTIVE_ERRORS
  if(ptr < lower)
  {
    std::cerr << "Out of bounds: ptr: " << ptr << ", lower: " << lower std::endl;

  #if SILENT_ERRORS
    error_file << "spatial_check: Out of bounds: ptr(" << ptr << "), lower(" << lower << ")" << std::endl;
  #else
    assert(0);
  #endif /* SILENT_ERRORS */
  }
#else /* DESCRIPTIVE_ERRORS */

  #if SILENT_ERRORS
  if(ptr < lower)
  {
    error_file << "spatial_check: Out of bounds: ptr(" << ptr << "), lower(" << lower << ")" << std::endl;
  }
  #else
  assert(ptr >= lower);
  #endif /* SILENT_ERRORS */
#endif /* DESCRIPTIVE_ERRORS */

#if DESCRIPTIVE_ERRORS
//  if(ptr > upper) {
  // We should only check until the upper... since the size is added to the
  // base... and comparing with ptr > upper implies that you can still write
  // while the pointer points to the upper limit... which is wrong.
  if(ptr >= upper)
  {
    std::cerr  << "spatial_check: Out of bounds: ptr(" << ptr << "), upper(" << upper << ")" << std::endl;
  #if SILENT_ERRORS
    error_file << "spatial_check: Out of bounds: ptr(" << ptr << "), upper(" << upper << ")" << std::endl;
  #else
    assert(0);
  #endif /* SILENT_ERRORS */
  }
#else /* DESCRIPTIVE_ERRORS */

  #if SILENT_ERRORS
  if(ptr >= upper) {
    error_file << "spatial_check: Out of bounds: ptr(" << ptr << "), upper(" << upper << ")" << std::endl;
  }
  #else
  assert(ptr < upper);
  #endif /* SILENT_ERRORS */
#endif /* DESCRIPTIVE_ERRORS */
}

void temporal_check(uint64_t lock, uint64_t key)
{
  if (LockDB.isDummyLock(lock)) return;

  if (key == 0)
  {
    std::cerr << ("key = 0; freed this pointer\n");
    assert(0);
  }

  if (LockDB[lock] != key)
  {
    std::cerr << "Temporal Error: Locks[" << lock << "] = "
              << LockDB[lock] << ", key = " << key
              << std::endl;
    assert(0);
  }
}

void remove_entry_from_addr(unsigned long long addr)
{
  MetaData md = TrackingDB.get_entry(addr);

  LockDB.remove_lock(md.lock);
  TrackingDB.remove_entry(addr);
}

void remove_entry(__Pb__v__Pe___Type input)
{
  remove_entry_from_addr(input.addr);
}

void create_entry_with_new_lock(unsigned long long addr, unsigned long long base, unsigned long size) {
  IntPair lock_key = LockDB.insert_lock();
  TrackingDB.create_entry(addr, base, base + size, lock_key.first, lock_key.second);
}

void lower_bound_check(unsigned long long ptr, unsigned long long addr) {
  IntPair lower_upper = TrackingDB.get_bounds(addr);
  assert(lower_upper.first == ptr);
}

void metadata_free_overload(unsigned long long ptr, unsigned long long addr)
{
#if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_free_overload\n");
#endif /* THREADX_DEBUG */

  metadata_check_entry(ptr, addr);

  // Also check if input.ptr is greater than base... in which case, free
  // won't be able to get the correct metadata... free can only be called
  // when the pointer points to the base of the allocation.
  lower_bound_check(ptr, addr);

  __Pb__v__Pe___Type input = { (void*)ptr, addr };
  remove_entry(input);
}

void metadata_realloc_overload_2(unsigned long long ptr, unsigned long long addr, unsigned long size) {
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_realloc_overload_2\n");
  #endif /* THREADX_DEBUG */

  // Update the bounds
  //v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(str.addr, reinterpret_cast<unsigned long long>(str.ptr), size);
  // FIXME: Might need to zero out the previous lock, since we are taking a new lock here...
  create_entry_with_new_lock(addr, ptr, size);
}

void metadata_realloc_overload_1(unsigned long long ptr, unsigned long addr) {
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_realloc_overload_1\n");
  #endif /* THREADX_DEBUG */
  // Since we are "reallocating", the str.ptr should either be NULL or
  // a valid entry.
  if((void*)ptr != NULL)
  {
    TrackingDB.isValidEntry(addr);
  }

  // Also check if ptr actually points to the lower bound of the allocation
  lower_bound_check(ptr, addr);
}

void metadata_create_entry_if_src_exists(unsigned long long dest, unsigned long long src) {
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_create_entry_if_src_exists\n");
  #endif /* THREADX_DEBUG */
  if(!TrackingDB.entryExists(src)) {
    return;
  }
  TrackingDB.copy_entry(dest, src);
}



void metadata_malloc_overload(unsigned long long addr, unsigned long long base, unsigned long size)
{
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_malloc_overload\n");
  threadx_file << "metadata_malloc_overload\n";
  #endif /* THREADX_DEBUG */

  create_entry_with_new_lock(addr, base, size);
}


void metadata_check_entry(unsigned long long ptr, unsigned long long addr)
{
#if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_check_entry\n");
  fflush(NULL);
#endif /* THREADX_DEBUG */

  MetaData md = TrackingDB.get_entry(addr);

  // Check if its a blank entry... that means this is possibly a NULL ptr
  // deref...
#if DESCRIPTIVE_ERRORS
  if(md.L == 0 && md.H == 0 && md.lock == 0 && md.key == 0) {
    std::cerr << ("check_entry: blank_entry dereferenced. NULL pointer deref\n");

    #if SILENT_ERRORS
    error_file << "check_entry: blank_entry derefed at addr(" << addr << ")" << std::endl;
    #else
    assert(0);
    #endif /* SILENT_ERRORS */
  }
#else /* DESCRIPTIVE_ERRORS */

  #if SILENT_ERRORS
  if(md.L == 0 && md.H == 0 && md.lock == 0 && md.key == 0) {
    error_file << "check_entry: blank_entry derefed at addr(" << addr << ")" << std::endl;
  }
  #else
  assert(!(md.L == 0 && md.H == 0 && md.lock == 0 && md.key == 0));
  #endif /* SILENT_ERRORS */

#endif /* DESCRIPTIVE_ERRORS */

#if DESCRIPTIVE_ERRORS
  // std::cerr << ("Temporal check\n");
#endif /* DESCRIPTIVE_ERRORS */

  temporal_check(md.lock, md.key);

#if DESCRIPTIVE_ERRORS
  //std::cerr << ("Spatial check\n");
#endif /* DESCRIPTIVE_ERRORS */
  spatial_check(ptr, md.L, md.H);
}

void metadata_array_bound_check_using_lookup(unsigned long long addr, unsigned long long index) {
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_array_bound_check_using_lookup\n");
  #endif /* THREADX_DEBUG */

  // This is a pointer variable, which is being accessed using a pntr arr ref exp.
  // So, do a temporal and spatial check
  // If its not a valid entry, then this pointer hasn't been initialized yet.
  TrackingDB.isValidEntry(addr);

  if (TrackingDB.isDummyBoundCheck(addr)) return;

  MetaData md = TrackingDB.get_entry(addr);

  // The check here is to see if the index is less than the size...
  // Not really, a spatial check like we typically do...
  // The index will give ptr + index*sizeof(type).
  // So, we simply have to check that its below the upper bound.
  assert(index < md.H);
  // It is possible for pointers using pntr arr refs to call this funciton, and use
  // negative array indices. The lower bound check handles that case.
  assert(index >= md.L);
  temporal_check(md.lock, md.key);
}


#if INT_ARRAY_INDEX
void metadata_array_bound_check(unsigned int size, int index) {
#else
void metadata_array_bound_check(unsigned int size, unsigned int index) {
#endif /* INT_ARRAY_INDEX */
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_array_bound_check\n");
  #endif /* THREADX_DEBUG */

  // \todo FIXME: Not checking if casting can still cause the read/write to exceed
  // the bounds.

#if DESCRIPTIVE_ERRORS
  if(index >= size) {
    std::cerr  << "array_bound_check: out of bounds: index(" << index << "), size(" << size << std::endl;

  #if SILENT_ERRORS
    error_file << "array_bound_check: out of bounds: index(" << index << "), size(" << size << std::endl;
  #else
    assert(0);
  #endif /* SILENT_ERRORS */
  }
#else /* DESCRIPTIVE_ERRORS */

  #if SILENT_ERRORS
  if(index >= size) {
    error_file << "array_bound_check: out of bounds: index(" << index << "), size(" << size << std::endl;
  }
  #else
  // index shouldn't be less than zero
  assert(index < size);
  #endif /* SILENT_ERRORS */
#endif /* DESCRIPTIVE_ERRORS */

#if 0
  MetaData md = TrackingDB[addr];

  spatial_check(index, md.L, md.H);
#endif
}


void metadata_create_entry_3(unsigned long long addr, unsigned long long base, unsigned long size) {
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_create_entry_3\n");
  #endif /* THREADX_DEBUG */

  // Pluck the lock and key from the top of scope stack...
  // Or, it would be passed in... this would eventually happen
  // since we would need a lock and key for each scope, or maybe even
  // each variable
  IntPair lock_key = LockDB.getTopOfSLK();
  TrackingDB.create_entry(addr, base, base + size, lock_key.first, lock_key.second);
/*
  struct MetaData md;
  md.L = base;
  md.H = base + size;
*/
}


void metadata_create_entry_4(unsigned long long addr, unsigned long long base, unsigned long size, unsigned long long lock)
{
#if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_create_entry_4\n");
#endif /* THREADX_DEBUG */

#if USE_DUMMY_LOCK
  if (lock == getDummyLock())
  {
    TrackingDB.create_entry(addr, base, base + size, lock, getDummyKey());
    return;
  }
#endif /* USE_DUMMY_LOCK */

  TrackingDB.create_entry(addr, base, base + size, lock, LockDB.getKey(lock));
}

void metadata_create_entry_dest_src(unsigned long long dest, unsigned long long src)
{
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_create_entry_dest_src\n");
  std::cerr << "dest: " << dest << " src: " << src << std::endl;
  #endif /* THREADX_DEBUG */

  if (src)
  {
    TrackingDB.isValidEntry(src);

    TrackingDB.copy_entry(dest, src);
  }
  else
  {
    TrackingDB.create_blank_entry(src);
  }
}

void metadata_create_dummy_entry(unsigned long long addr)
{
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_create_dummy_entry\n");
  #endif /* THREADX_DEBUG */

  TrackingDB.create_dummy_entry(addr);
}

void metadata_execAtLast()
{
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_execAtLast\n");
  #endif /* THREADX_DEBUG */

  std::cerr << ("execAtLast: Begin\n");

  LockDB.checkBeforeExit();
  LockDB.removeFromSLK();

  std::cerr << ("execAtLast: End\n");
}

void metadata_checkInitInfo(long long /*lock*/, unsigned long long /*addr*/)
{
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_checkInitInfo\n");
  #endif /* THREADX_DEBUG */
}

void metadata_updateInitInfo(long long /*lock*/, unsigned long long /*addr*/)
{
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_updateInitInfo\n");
  #endif /* THREADX_DEBUG */

  //InitInfo* init_map= &InitVec[lock];
  //(*init_map)[addr] = 1;
}

void metadata_ExitScope()
{
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_ExitScope\n");
  #endif /* THREADX_DEBUG */

  // IntPair lock_key = LockDB.getTopOfSLK();
  LockDB.removeFromSLK();
}

void metadata_EnterScope()
{
  #if THREADX_DEBUG
  std::cerr << ("\t\t\t\tmetadata_EnterScope\n");
  #endif /* THREADX_DEBUG */

  /*IntPair lock_key =*/ LockDB.insertIntoSLK();
}

void metadata_execAtFirst()
{
  std::cerr << ("execAtFirst: Begin\n");

  #if SILENT_ERRORS
  error_file.open("error.txt");
  #endif /* SILENT_ERRORS */

  // Now insert a single lock into the ScopeLocksAndKeys.
  // This will be the scope stack start for the main function --
  // if this function is called from main -- and it'll be the default
  // one when the stack level checks are not implemented.
  /*IntPair inserted =*/ LockDB.insertIntoSLK();

  std::cerr << ("execAtFirst: End\n");
}

unsigned int free_index = 0;

void metadata_push_to_stack(unsigned long long addr) {
  static bool first = true;
  if(first) {
    first = false;
    argMetadata.resize(8);
  }

  if(argMetadata.size() < free_index) {
    argMetadata.resize(free_index + 1);
  }
  argMetadata[free_index] = addr;
  free_index++;

#if STACK_DEBUG
  std::cerr << "push: addr: " << addr << ", free_index: " << free_index << std::endl;
#endif /* STACK_DEBUG */
}

unsigned long long metadata_get_from_stack(unsigned int index)
{
  // free_index is one greater than the current highest arg
  // inserted... thanks to the post increment.
  // Both index and free_index start from zero.
  unsigned long long cached = argMetadata[free_index - 1 - index];

#if STACK_DEBUG
  std::cerr << "get: index: "   << index
            << ", free_index: " << free_index
            << ", getting: "    << argMetadata[free_index - 1 - index])
            << std::endl;
#endif /* STACK_DEBUG */

  return cached;
}

unsigned long long metadata_pop_from_stack(unsigned int evict) {
  assert(free_index >= evict);
  // free_index is one head of the top arg
  unsigned long long top = argMetadata[free_index - 1];
#if STACK_DEBUG
  std::cerr << "pop: free_index: " << free_index << ", top: " << top << std::endl;
#endif /* STACK_DEBUG */

  free_index--;
  // we have evicted one arg... so, decrement evict
  evict--;

  // Now, free_index points to the arg freed just now
  // We decrement free_index by evict...
  free_index -= evict;

#if STACK_DEBUG
  std::cerr << "pop: free_index: " << free_index << ", evict: " << evict << std::endl;
#endif /* STACK_DEBUG */

  return top;
}

#ifdef NO_MANGLE
#ifdef __cplusplus
}
#endif
#endif

// #include "metalib_alt.C"
