//
// shared heap protection (Workzone)
//

#include <stdio.h>
#include <assert.h>

#include "rtedsync.h"
#include "workzone.h"

workzone_policy_t* workzone = 0;
upc_lock_t*        sharedptrlock = 0;

void rted_UpcAllInitWorkzone(void)
{
  assert(!workzone);

  workzone = wzp_all_alloc();
  sharedptrlock = upc_all_lock_alloc();
}

void rted_UpcEnterWorkzone(void)
{
  assert(workzone);

  wzp_enter(workzone);
}

void rted_UpcExitWorkzone(void)
{
  assert(workzone);

  wzp_exit(workzone);
}

void rted_UpcBeginExclusive(void)
{
  assert(workzone);

  wzp_beginExperiment(workzone);
}

void rted_UpcEndExclusive(void)
{
  assert(workzone);

  wzp_endExperiment(workzone);
}

void rted_staySafe(void)
{
  assert(workzone);

  wzp_staySafe(workzone);
}

void rted_sync_log(void)
{
  if (MYTHREAD != 0) return;

  printf("  workzone = %p\n", (void*)workzone);
  if (workzone == 0) return;

  printf("    rw_lock = %p\n", (void*)(workzone->rw));
  printf("    flag = %lu\n", workzone->warning_flag);
  if (workzone->rw == 0) return;

  printf("      readers = %lu\n", workzone->rw->readers);
  printf("      writer = %lu\n", workzone->rw->current_writer);
}

void rted_EnterSharedPtr(rted_Address addr)
{
  upc_lock(sharedptrlock);
}

void rted_ExitSharedPtr(rted_Address addr)
{
  upc_unlock(sharedptrlock);
}

void rted_barrier(void)
{
  rted_UpcExitWorkzone();
  upc_barrier;
  rted_UpcEnterWorkzone();
}
