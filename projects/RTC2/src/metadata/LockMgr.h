#ifndef _LOCKMGR_H
#define _LOCKMGR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <climits> //RMM
#include <math.h>
#include <map>
#include <stack>
#include <list>
#include <fstream>
#include <iostream>
#include <queue>
#include <fcntl.h>

// The organization of TrackingDB and Locks is described here.
// TrackingDB holds the metadata for each pointer.
// Locks holds the temporal information for each allocation/scope.
// We would need an unlimited size TrackingDB to hold all the pointers
// that currently being tracked.
// Locks needs to be extensible as well since its tough to predict
// the exact number of dynamic locks required, statically. Allocating a
// a huge structure might be wasteful.

// For the TrackingDB we use a hash map since we would be using
// the address of the pointer to actually find and update its metadata.
// For the Locks, we can use a structure that can dynamically increase in size
// without having to be reallocated/moved.

// MetadataMgr is the class that holds the data and functions for the TrackingDB
// LockMgr is the class that holds the data and functions for the Locks

// In the LockMgr, we include the dynamically increasing locks part.
// This could be implemented in another subclass as well, but we'll leave it in LockMgr
// for now. Given that we support N-bit locks (N being 32-bit or 64-bit... basically the
// size of the variable used as lock index). The MSB M bits are used to select the bin
// in which we are searching for the lock. The next M bits give the second level bin
// that we should look for. Its like a tree structure, with 2^M bins at each level. We could
// skew this, instead of a uniform M-bit bin lookup, but that should be a parameter.
// Finally, the last N - 2*M bits is the size of the actual lock array. If no such array exists
// then we should make sure that N - 2*M points to the base of the array and that the current
// lookup allows for a new lock to be allocated. Once that is known, we can allocate a new lock
// array and set the lock to a given value, or zero it out depending on what is needed.


// So, lets specify the macro names for this.
#define BIN1_BITS 8
#define BIN2_BITS 8
#define BIN3_BITS 12
// BIN1_BITS + BIN2_BITS + BIN3_BITS = total number of bits being used for supporting locks
// i.e. TOTAL_BITS = BIN1_BITS + BIN2_BITS + BIN3_BITS
// max number of locks supported = 2^TOTAL_BITS
// Instead of using the "pow" function, the masks can
// be precalculated here using bit shifts.
#define BIN1_SIZE (1 << BIN1_BITS)
#define BIN2_SIZE (1 << BIN2_BITS)
#define BIN3_SIZE (1 << BIN3_BITS)
#define BIN1_MASK (BIN1_SIZE - 1)
#define BIN2_MASK (BIN2_SIZE - 1)
#define BIN3_MASK (BIN3_SIZE - 1)
#define SHIFT1 (BIN2_BITS + BIN3_BITS)
#define SHIFT2 BIN3_BITS
// SHIFT3 is zero.
// The masks will extract the required bits from a lock index.

#define START_LOCK BIN3_SIZE
#define START_KEY 100


// Should be in the new lock structure. Inserted here
// to compile code below with new MetaDataMgr

//--------------------------------
struct MetaData
{
  uint64_t L;
  uint64_t H;
  uint64_t lock;
  uint64_t key;
};


typedef std::map<uint64_t, struct MetaData> MetaMap;

typedef std::pair<uint64_t*, uint64_t> PtrIntPair;
typedef std::pair<uint64_t, uint64_t> IntPair;
typedef std::stack<PtrIntPair> PtrIntPairStack;
typedef std::stack<IntPair> IntPairStack;
typedef std::list<uint64_t> IntList;
// This is the map that stores initialization information
// for each lock
typedef std::map<uint64_t, unsigned int> InitInfo;

static inline
uint64_t getDummyLock() { return DUMMY_LOCK; }

static inline
uint64_t getDummyKey() { return DUMMY_KEY; }

class LockMgr
{
  //uint64_t Locks[NUM_LOCKS];
  uint64_t*** Locks;
  uint64_t LargestUnusedKey;
  uint64_t LargestUnusedLock;
  IntPairStack ScopeLocksAndKeys;
  IntList FreeLocks;
  IntList UsedLocks;

  public:

  LockMgr() {

    // Allocate first dim
    Locks = (uint64_t***)malloc(sizeof(uint64_t**)*BIN1_SIZE);

    // Allocate second dim
    for(int first_index = 0; first_index < BIN1_SIZE; first_index++) {
      Locks[first_index] = (uint64_t**)malloc(sizeof(uint64_t*)*BIN2_SIZE);
      for(int second_index = 0; second_index < BIN2_SIZE; second_index++) {
        Locks[first_index][second_index] = NULL;
      }
    }

    // Allocate only the first BIN3_SIZE of entries.
    // Rest will allocated on demand.
    // The first BIN3_SIZE will house special cases...
    // DUMMY_LOCK is in this set... These locks won't be used
    // for actual temporal tracking
    allocate_locks(0,0);

    // Zero out unused lock and key counters
    LargestUnusedLock = START_LOCK;
    LargestUnusedKey = START_KEY;

  }

  ~LockMgr() {
    // Remove the first bin in the third dim BIN3_SIZE that is left out.
    free(Locks[0][0]);

    // Remove the second dim
    for(int first_index = 0; first_index < BIN1_SIZE; first_index++) {
      free(Locks[first_index]);
    }

    // Remove the first dim
    free(Locks);
  }

  void checkBeforeExit()
  {
//~ #if TEMP_DISABLED
    assert(FreeLocks.size() == (LargestUnusedLock - START_LOCK - 1));
    assert(ScopeLocksAndKeys.size() == 1);
//~ #endif /* TEMP_DISABLED */
  }

  uint64_t get_first_index(uint64_t lock_index) {
    // shift the lock_index by BIN2_BITS + BIN3_BITS
    uint64_t lock_index_shifted = lock_index >> SHIFT1;
    // Now mask these bits with the bin1_mask
    uint64_t first_index = lock_index_shifted & BIN1_MASK;
    return first_index;
  }

  uint64_t get_second_index(uint64_t lock_index) {
    // shift the lock_index by BIN3_BITS;
    uint64_t lock_index_shifted = lock_index >> SHIFT2;
    // Now mask these bits with the bin2_mask
    uint64_t second_index = lock_index_shifted & BIN2_MASK;
    return second_index;
  }

  uint64_t get_third_index(uint64_t lock_index) {
    // No need to shift, just mask with bin3_mask
    uint64_t third_index = lock_index & BIN3_MASK;
    return third_index;
  }

  void allocate_locks(uint64_t first_index, uint64_t second_index) {
    Locks[first_index][second_index] = (uint64_t*)malloc(sizeof(uint64_t)*BIN3_SIZE);
    memset(Locks[first_index][second_index], 0,
            sizeof(uint64_t)*BIN3_SIZE);
  }

  uint64_t& operator[](const uint64_t &lock_index) {
    uint64_t first_index = get_first_index(lock_index);
    uint64_t second_index = get_second_index(lock_index);
    uint64_t third_index = get_third_index(lock_index);
    assert(Locks[first_index][second_index]);
    return Locks[first_index][second_index][third_index];
  }

  void allocateLockIfNecessary(uint64_t lock_index) {
    uint64_t first_index = get_first_index(lock_index);
    uint64_t second_index = get_second_index(lock_index);
    uint64_t third_index = get_third_index(lock_index);
    if(Locks[first_index][second_index]) {
      return;
    }
    else {
      assert(!third_index);
      allocate_locks(first_index, second_index);
      return;
    }
  }

  void set_key(uint64_t new_lock_index, uint64_t new_key) {
    uint64_t first_index = get_first_index(new_lock_index);
    uint64_t second_index = get_second_index(new_lock_index);
    uint64_t third_index = get_third_index(new_lock_index);
    Locks[first_index][second_index][third_index] = new_key;
    assert(Locks[first_index][second_index][third_index] == new_key);
  }

  IntPair insert_lock() {
    uint64_t new_lock_index;
    if(FreeLocks.size()) {
      new_lock_index = FreeLocks.back();
      FreeLocks.pop_back();
    }
    else {
      // Get LargestUnusedLock
      new_lock_index = LargestUnusedLock++;
      allocateLockIfNecessary(new_lock_index);
    }

    uint64_t new_key = LargestUnusedKey++;
    // Write the new key at the new_lock_index
    //Locks[new_lock_index] = new_key;
    set_key(new_lock_index, new_key);

#if DESCRIPTIVE_ERRORS
    printf("inserted: (%llu, %llu)\n", new_lock_index, new_key);
#endif /* DESCRIPTIVE_ERRORS */

    // UsedLocks will grow quite large. Might be wasteful
#if 0
    // This will be a used lock... so enter it into the UsedLocks.
    UsedLocks.push_back(new_lock_index);
#endif

    return std::make_pair(new_lock_index, new_key);
  }

  #if 0
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
  #endif

  void remove_lock(uint64_t lock_index)
  {
#if USE_DUMMY_LOCK
    // Do not remove dummy locks if in use
    if (lock_index == getDummyLock()) return;
#endif /* USE_DUMMY_LOCK */

#if DESCRIPTIVE_ERRORS
    printf("removing: (%llu, %llu)\n", lock_index, Locks[lock_index]);
#endif /* DESCRIPTIVE_ERRORS */

    // Now, we zero out the lock_index location
    set_key(lock_index, 0);

    // Remove lock_index from UsedLocks
    //removeFromUsedLocks(lock_index);

    // Add it to the FreeLocks
    FreeLocks.push_back(lock_index);

#if DEBUG
    printf("remove_entry: FreeLocks.size(): %lu\n", FreeLocks.size());
#endif /* DEBUG */

    return;
  }

  IntPair getTopOfSLK()
  {
#if USE_DUMMY_LOCK
    IntPair lock_key = ScopeLocksAndKeys.top();
    assert(lock_key.first != getDummyLock());
#endif /* USE_DUMMY_LOCK */

    return ScopeLocksAndKeys.top();
  }

  long long L_Ret_getTopLock() {
    IntPair slk_top = getTopOfSLK();
    return slk_top.first;
  }

  long long L_Ret_getTopKey() {
    IntPair slk_top = getTopOfSLK();
    return slk_top.second;
  }

  uint64_t getKey(uint64_t lock_index) {
    uint64_t first_index = get_first_index(lock_index);
    uint64_t second_index = get_second_index(lock_index);
    uint64_t third_index = get_third_index(lock_index);
    return Locks[first_index][second_index][third_index];
  }

  void removeFromSLK() {

    // Get the lock that we are removing from SLK
    IntPair lock_key = getTopOfSLK();
    uint64_t lock = lock_key.first;
    // uint64_t key = lock_key.second;

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
    //printf("insertIntoSLK:top: (%llu, %llu)\n", new_lock.first, new_lock.second);

    return new_lock;
  }

  void printList(IntList& elems) {
    printf("List:");
    for(IntList::iterator it = elems.begin();
            it != elems.end(); ++it) {
        printf("%lu,",*it);
    }
    printf("\n");
    printf("End\n");
  }

  bool isDummyLock(uint64_t lock_index)
  {
#if USE_DUMMY_LOCK
    return lock_index == getDummyLock();
#else
    return false;
#endif /* USE_DUMMY_LOCK */
  }

};
#endif /* _LOCKMGR_H */
