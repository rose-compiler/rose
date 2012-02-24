#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <map>
#include <stack>
#include <list>

#include "MemoryManager.h"

#if 1
#ifdef __cplusplus
extern "C" {
#endif
#endif


#define START_KEY 100
#define NUM_LOCKS 100

struct __Pb__v__Pe___Type
{
  void *ptr;
  unsigned long long addr;
}
;

typedef uint64_t KeyType;

//--------------------------------
struct MetaData {
  uint64_t L;
  uint64_t H;
  uint64_t lock;
  KeyType  key;
};

typedef std::map<uint64_t, struct MetaData> MetaMap;
MetaMap TrackingDB;

struct LocktableEntry
{
  KeyType                       key;
  std::map<Address, MemoryType> memtypes;
};

LocktableEntry Locks[NUM_LOCKS];
KeyType        LargestUnusedKey;

typedef std::pair<uint64_t*, uint64_t> PtrIntPair;
typedef std::pair<uint64_t, uint64_t> IntPair;
typedef std::stack<PtrIntPair> PtrIntPairStack;
typedef std::stack<IntPair> IntPairStack;
typedef std::list<uint64_t> IntList;

//PtrIntPairStack ScopeLocksAndKeys;
IntPairStack ScopeLocksAndKeys;
IntList FreeLocks;
IntList UsedLocks;

IntPair insert_lock() {

  assert(FreeLocks.size() != 0);

  uint64_t new_lock_index = FreeLocks.back();
  uint64_t new_key = LargestUnusedKey++;
  FreeLocks.pop_back();

  // Write the new_key at the new_lock_index
  Locks[new_lock_index] = new_key;

  // This will be a used lock... so enter it into the UsedLocks.
  UsedLocks.push_back(new_lock_index);

  printf("inserted: (%llu, %llu)\n", new_lock_index, new_key);

  return std::make_pair(new_lock_index, new_key);
}

void removeFromUsedLocks(uint64_t val) {

  // Start searching from the back... since
  // we always push and pop from the back
  IntList::iterator i;
  for(i = UsedLocks.begin(); i != UsedLocks.end(); i++) {
    uint64_t curr = *i;
    if(curr == val) {
      break;
    }
  }

  if(i == UsedLocks.end()) {
    printf("Can't remove %llu from UsedLocks. Not found!\n", val);
    assert(0);
  }

  UsedLocks.erase(i);

  return;

}

void remove_lock(uint64_t lock_index) {

  printf("removing: (%llu, %llu)\n", lock_index, Locks[lock_index]);

  // Now, we zero out the lock_index location
  Locks[lock_index] = 0;

  // Remove lock_index from UsedLocks
  removeFromUsedLocks(lock_index);

  // Add it to the FreeLocks
  FreeLocks.push_back(lock_index);

  return;
}

IntPair getTopOfSLK() {
	return ScopeLocksAndKeys.top();
}

uint64_t getTopLock() {
	IntPair slk_top = getTopOfSLK();
	return slk_top.first;
}

uint64_t getTopKey() {
	IntPair slk_top - getTopOfSLK();
	return slk_top.second;
}

void removeFromSLK() {
	
	// Get the lock that we are removing from SLK
	IntPair lock_key = getTopOfSLK();
	uint64_t lock = lock_key.first;
	uint64_t key = lock_key.second;

	// Remove the lock from the UsedLocks and push it
	// into FreeLocks. Also zero it out. All done by
	// remove_lock
	remove_lock(lock);

	// Now, remove the lock from SLK itself
	ScopeLocksAndKeys.pop();
}

IntPair insertIntoSLK() {

  IntPair new_lock = insert_lock();
  ScopeLocksAndKeys.push(new_lock);
  printf("insertIntoSLK:top: (%llu, %llu)\n", new_lock.first, new_lock.second);

  return new_lock;
}

void execAtFirst() {

  printf("execAtFirst: Begin\n");

  LargestUnusedKey = START_KEY;
  printf("LargestUnusedKey: %llu\n", LargestUnusedKey);

  // zero out the Locks array. No lock is used to begin with.
  memset(Locks, 0, sizeof(Locks));

  // enter all the locks into FreeLocks
  // Locks numbers are same as indices
  for(unsigned int index = 0; index < NUM_LOCKS; index++) {
    FreeLocks.push_back(index);
  }

  // Now insert a single lock into the ScopeLocksAndKeys.
  // This will be the scope stack start for the main function --
  // if this function is called from main -- and it'll be the default
  // one when the stack level checks are not implemented.
  IntPair inserted = insertIntoSLK();

  printf("execAtFirst: End\n");

}

void execAtLast() {

  printf("execAtLast: Begin\n");
  // The size of the FreeLocks should be one less than than NUM_LOCKS
  // since we will still be in scope for the current function (main).
  assert(FreeLocks.size() == (NUM_LOCKS - 1));
  assert(ScopeLocksAndKeys.size() == 1);

  // Remove the remaining lock from SLK.
  removeFromSLK();

  // Now, used locks should be zero.
  assert(UsedLocks.size() == 0);
  printf("execAtLast: End\n");
}

uint64_t find_lock(unsigned long long base) {
  // Some number
  return 0;
}

uint64_t find_key(uint64_t lock) {
  // Some number
  return 0;
}

void create_blank_entry(unsigned long long addr) {
  struct MetaData md;
  md.L = 0;
  md.H = 0;
  md.lock = 0;
  md.key = 0;
  TrackingDB[addr] = md;
}

bool isValidEntry(unsigned long long addr) {
  MetaMap::iterator iter = TrackingDB.find(addr);

  return (iter != TrackingDB.end());
}

void v_Ret_create_entry_UL_Arg_UL_Arg(unsigned long long dest,unsigned long long src) {

  if(src) {
    assert(isValidEntry(src));

    TrackingDB[dest] = TrackingDB[src];
  }
  else {
    printf("create_entry: Source is empty. Blank metadata at dest\n");
    create_blank_entry(dest);
  }
}


void v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(unsigned long long addr,unsigned long long base,unsigned long size) {


	// Pluck the lock and key from the top of scope stack...
	// Or, it would be passed in... this would eventually happen
	// since we would need a lock and key for each scope, or maybe even
	// each variable
	//IntPair lock_key = insert_lock();
	IntPair lock_key = getTopOfSLK(); 

  struct MetaData md;
  md.L = base;
  md.H = base + size;

  md.lock = lock_key.first;
  md.key = lock_key.second;

  TrackingDB[addr] = md;
}

void spatial_check(unsigned long long ptr, unsigned long long lower, unsigned long long upper) {
  assert(ptr >= lower);
  assert(ptr <= upper);
}

void temporal_check(uint64_t lock, uint64_t key) {
  assert(key != 0); // To detect double free
  assert(Locks[lock] == key);
}

void v_Ret_check_entry_UL_Arg_UL_Arg(unsigned long long ptr, unsigned long long addr) {
  struct MetaData md = TrackingDB[addr];
  temporal_check(md.lock, md.key);
  spatial_check(ptr, md.L, md.H);
}

void remove_entry(struct __Pb__v__Pe___Type input) {
  // clear the lock
  struct MetaData md = TrackingDB[input.addr];
  remove_lock(md.lock);

  // clear the struct.
  md.L = 0; md.H = 0; md.lock = 0; md.key = 0;
  TrackingDB[input.addr] = md;
}

void create_entry_with_new_lock(unsigned long long addr, unsigned long long base, unsigned long size) {

	struct MetaData md;
	md.L = base;
	md.H = base + size;

	IntPair lock_key = insert_lock();

	md.lock = lock_key.first;
	md.key = lock_key.second;

	TrackingDB[addr] = md;
}


struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_malloc_overload_Ul_Arg(unsigned long size) {

	struct __Pb__v__Pe___Type output;
	output.ptr = malloc(size);
	output.addr = reinterpret_cast<unsigned long long>(&output.ptr);
	create_entry_with_new_lock(output.addr, reinterpret_cast<unsigned long long>(output.ptr), size);
	#ifdef DEBUG	
	printf("malloc overload\n");
	printf("output.ptr: %llu, output.addr: %llu\n", reinterpret_cast<unsigned long long>(output.ptr),
													output.addr);
	#endif
	return output;
}

struct __Pb__v__Pe___Type __Pb__v__Pe___Type_Ret_realloc_overload___Pb__v__Pe___Type_Arg_Ul_Arg(struct __Pb__v__Pe___Type str, unsigned long size) {
	
	// Since we are "reallocating", the str.ptr should either be NULL or 
	// a valid entry.
	if(str.ptr != NULL) {
		assert(isValidEntry(str.addr));
	}

	// Do a realloc and update the bounds.
	str.ptr = realloc(str.ptr, size);
	// Update the bounds
	//v_Ret_create_entry_UL_Arg_UL_Arg_Ul_Arg(str.addr, reinterpret_cast<unsigned long long>(str.ptr), size);
	// FIXME: Might need to zero out the previous lock, since we are taking a new lock here...
	create_entry_with_new_lock(str.addr, reinterpret_cast<unsigned long long>(str.ptr), size);

	return str;
}


void v_Ret_free_overload___Pb__v__Pe___Type_Arg(struct __Pb__v__Pe___Type input) {
  v_Ret_check_entry_UL_Arg_UL_Arg((unsigned long long)input.ptr, input.addr);
  remove_entry(input);
  free(input.ptr);

}

#if 1
#ifdef __cplusplus
}
#endif
#endif

//------------------------------
