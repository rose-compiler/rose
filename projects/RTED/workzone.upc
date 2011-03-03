
#include <upc.h>
#include <assert.h>
#include <stdio.h>

#include "workzone.h"

#define UNDEFINED_WRITER ((size_t)-1)

static inline
void rw_assert_read_mode(rw_lock_t* rwl)
{
  assert(rwl->readers > 0);
  assert(rwl->current_writer == UNDEFINED_WRITER);
}

static inline
void rw_assert_this_writes(rw_lock_t* rwl)
{
  assert(rwl->current_writer - MYTHREAD == 0);
}

static inline
void rw_assert_unused(rw_lock_t* rwl)
{
  assert(rwl->readers == 0);
  assert(rwl->current_writer == UNDEFINED_WRITER);
}

static
void rw_lock_write(rw_lock_t* rwl)
{
  // busy wait until everybody is in safety
  for (;;)
  {
    // no lock, in order not to disturb with exiting readers
    while (rwl->readers);

    upc_lock(rwl->lock);
    if (rwl->readers == 0) break;
    upc_unlock(rwl->lock);
  }

  rwl->current_writer = MYTHREAD;
}

static
void rw_unlock_write(rw_lock_t* rwl)
{
  rw_assert_this_writes(rwl);

  rwl->current_writer = UNDEFINED_WRITER;
  upc_unlock(rwl->lock);
}

static
void rw_lock_read(rw_lock_t* rwl)
{
  upc_lock(rwl->lock);
  assert(rwl->current_writer == UNDEFINED_WRITER);
  ++rwl->readers;
  upc_unlock(rwl->lock);
}

static
void rw_unlock_read(rw_lock_t* rwl)
{
  rw_assert_read_mode(rwl);

  upc_lock(rwl->lock);
  --rwl->readers;
  upc_unlock(rwl->lock);
}

//
// workzone policy
//

static
size_t wzp_raiseWarning(workzone_policy_t* pol)
{
  size_t res = 0;

  // \todo consider using upc_lock_attempt
  upc_lock(pol->flag_lock);
  if (pol->warning_flag == 0) res = pol->warning_flag = 1;
  upc_unlock(pol->flag_lock);

  return res;
}

static
void wzp_clearWarning(workzone_policy_t* pol)
{
  assert(pol->warning_flag);
  pol->warning_flag = 0;
}

void wzp_beginExperiment(workzone_policy_t* pol)
{
  // the calling thread is still a reader
  rw_assert_read_mode(pol->rw);

  int myturn = wzp_raiseWarning(pol);

  while (!myturn)
  {
    wzp_staySafe(pol);
    myturn = wzp_raiseWarning(pol);
  }

  rw_unlock_read(pol->rw);
  rw_lock_write(pol->rw);
}

void wzp_endExperiment(workzone_policy_t* pol)
{
  rw_assert_this_writes(pol->rw);

  wzp_clearWarning(pol);
  rw_unlock_write(pol->rw);
  rw_lock_read(pol->rw);
}

void wzp_staySafe(workzone_policy_t* pol)
{
  // early return if nothing is going on
  if (!pol->warning_flag) return;

  rw_unlock_read(pol->rw);

  // we cannot use upc_signal/upc_wait b/c not all threads might get here
  //   i.e., if they have already exited the work zone.
  while (pol->warning_flag) ; // busy wait

  rw_lock_read(pol->rw);
}

void wzp_enter(workzone_policy_t* pol)
{
  rw_lock_read(pol->rw);
}

void wzp_exit(workzone_policy_t* pol)
{
  rw_unlock_read(pol->rw);
}

workzone_policy_t* wzp_all_alloc()
{
  rw_lock_t*         rw = upc_all_alloc(1, sizeof(rw_lock_t));
  workzone_policy_t* pol = upc_all_alloc(1, sizeof(workzone_policy_t));

  if (MYTHREAD == 0)
  {
    rw->readers = 0;
    rw->current_writer = UNDEFINED_WRITER;
    pol->warning_flag = 0;
    pol->rw = rw;
  }

  pol->flag_lock = upc_all_lock_alloc();
  pol->rw->lock = upc_all_lock_alloc();

  return pol;
}

void wzp_free(workzone_policy_t* pol)
{
  rw_assert_unused(pol->rw);

  upc_lock_free(pol->rw->lock);
  upc_lock_free(pol->flag_lock);
  upc_free(pol->rw);
  upc_free(pol);
}
