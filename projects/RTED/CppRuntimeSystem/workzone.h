/// \file    workzone.h
/// \brief   implements lock abstractions. i.e. a simple reader/writer lock
///          and a cooperative safe workzone.
/// \email   peter.pirkelbauer@llnl.gov

#ifndef _WORKZONE_UPH
#define _WORKZONE_UPH

#include <upc.h>
#include <stdio.h>
#include <stdlib.h>


/// \brief a simple reader writer lock, subject to writer starvation
///        The acquisiation of the writer lock is implemented using busy
///        waiting.
struct rw_lock
{
  size_t      readers;
  size_t      current_writer;
  upc_lock_t* lock;
};

typedef shared struct rw_lock rw_lock_t;

/// \brief   synchronizes concurrent shared operations
/// \details multiple readers are constantly operating in a workzone; a
///          participant intending an unsafe operation raises a
///          signal (beginExperiment), acquires a write lock (rw) on the workzone,
///          and waits until the workzone is clear (rw.readers == 0). readers
///          noticing the signal (checked by staySafe) exit the lab and
///          wait in a safe zone (staySafe) until the experiment is over
///          (endExperiment). Then all participants return to operate
///          concurrently.
///          RTED for UPC uses this scheme to protect threads from race
///          conditions on upc_free and violation output. As long as all
///          threads perform heap acquisitions, heap reads, and heap writes,
///          every thread can operate concurrently in the work zone; only
///          upc_frees (and uninterrupted console output) require other threads
///          to exit the workzone and wait until the operation has finished.
struct workzone_policy
{
  rw_lock_t*     rw;
  upc_lock_t*    flag_lock;
  size_t         warning_flag;
};

typedef shared struct workzone_policy workzone_policy_t;

void wzp_beginExperiment(workzone_policy_t* pol);
void wzp_endExperiment(workzone_policy_t* pol);
void wzp_staySafe(workzone_policy_t* pol);
void wzp_enter(workzone_policy_t* pol);
void wzp_exit(workzone_policy_t* pol);

workzone_policy_t* wzp_all_alloc(void);
void wzp_free(workzone_policy_t* pol);

#endif /* _WORKZONE_UPH */
