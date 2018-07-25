#ifndef _TRACKING_DB_H
#define _TRACKING_DB_H

#include <iostream>

#include "LockMgr.h"

#if THREADX_DEBUG
extern std::ofstream threadx_file; // \pp moved definition to C file
#endif /* THREADX_DEBUG */

#if SILENT_ERRORS
extern std::ofstream error_file; // \pp moved definition to C file
#endif /* SILENT_ERRORS */


class MetaDataMgr
{
    MetaMap Tracker;

  public:
    void copy_entry(uint64_t dest, uint64_t src) {
      isValidEntry(src);
      Tracker[dest] = get_entry(src);
    }

    void create_entry(uint64_t addr, uint64_t lower, uint64_t upper,
              uint64_t lock, uint64_t key) {
      struct MetaData md;
      md.L = lower; md.H = upper; md.lock = lock; md.key = key;
      set_entry(addr, md);
    }

    void create_blank_entry(uint64_t addr) {
      struct MetaData md;
      md.L = 0; md.H = 0; md.lock = 0; md.key = 0;
      set_entry(addr, md);
    }

    void create_dummy_entry(uint64_t addr) {
      struct MetaData md;
      md.L = DUMMY_LOWER_BOUND; md.H = DUMMY_UPPER_BOUND;
      md.lock = getDummyLock(); md.key = getDummyKey();
      set_entry(addr, md);
    }

    void remove_entry(uint64_t addr) {
      isValidEntry(addr);
      // Zero out the entry before erasing it
      create_blank_entry(addr);
      Tracker.erase(addr);
    }

    IntPair get_bounds(uint64_t addr) {
      struct MetaData md = get_entry(addr);
      return std::make_pair(md.L, md.H);
    }

    IntPair get_lock_and_key(uint64_t addr) {
      struct MetaData md = get_entry(addr);
      return std::make_pair(md.lock, md.key);
    }

    void isValidEntry(uint64_t addr) {
      assert(Tracker.find(addr) != Tracker.end());
    }

    bool entryExists(uint64_t addr) {
      return (Tracker.find(addr) != Tracker.end());
    }

    struct MetaData get_entry(uint64_t addr) {
      return Tracker[addr];
    }

    void set_entry(uint64_t addr, struct MetaData md) {
      Tracker[addr] = md;
    }

    void print_entry(uint64_t addr, uint64_t lower, uint64_t upper, uint64_t lock, uint64_t key)
    {
      std::cerr << "[" << (void*) addr << "]:(l)" << (void*)lower
                << " (h)" << (void*)upper << " (s)" << upper - lower
                << " (l)" << lock << " (k)" << key
                << std::endl;
    }

    void print_TrackingDB()
    {
      std::cerr << ("Printing TrackingDB\n");
      MetaMap::iterator it = Tracker.begin();
      for(; it != Tracker.end(); it++) {
        uint64_t addr = it->first;
        MetaData md = it->second;
        print_entry(addr, md.L, md.H - md.L, md.lock, md.key);
      }
      std::cerr << ("Done Printing\n");
    }

    // \pp why do we need it ?
    bool isDummyBoundCheck(uint64_t addr)
    {
#if USE_DUMMY_BOUNDS
      struct MetaData md = get_entry(addr);

      // Skip the checks
      return (md.L == DUMMY_LOWER_BOUND && md.H == DUMMY_UPPER_BOUND);
#else
      return false;
#endif /* USE_DUMMY_BOUNDS */

    }
};

#endif /* _TRACKING_DB_H */
