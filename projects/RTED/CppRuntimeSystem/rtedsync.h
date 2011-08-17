/// \file  rtedsync.h
/// \brief implements an RTED layer over the workzone implementation
///
/// \email peter.pirkelbauer@llnl.gov

#ifndef RTED_SYNC_H
#define RTED_SYNC_H

#include "ptrops.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if WITH_UPC

/// \brief releases the heap read lock
void rted_UpcEnterWorkzone(void);

/// \brief acquires the heap read lock
void rted_UpcExitWorkzone(void);

/// \brief starts an exclusive heap operation (i.e., upc_free)
void rted_UpcBeginExclusive(void);

/// \brief ends an exclusive heap operation (i.e., upc_free)
///        see also rted_UpcBeginExclusive
void rted_UpcEndExclusive(void);

/// \brief waits until a pending exclusive operation has finished
void rted_staySafe(void);

/// \brief initiakizes the workzone
void rted_UpcAllInitWorkzone(void);

/// \brief prints some status information to stdout, if invoked by Thread 0
void rted_sync_log(void);

/// \brief rted barrier
void rted_barrier(void);

/// \brief marks the begin of shared pointer to shared processing
void rted_EnterSharedPtr(rted_Address addr);

/// \brief marks the end of shared pointer to shared processing
void rted_ExitSharedPtr(rted_Address addr);

#else /* WITH_UPC */

#define UNUSEDARG(X) ((void) &(X)) /* we write for C and C++ compilers ;) */

static inline
void rted_UpcEnterWorkzone(void)
{}

static inline
void rted_UpcExitWorkzone(void)
{}

static inline
void rted_UpcBeginExclusive(void)
{}

static inline
void rted_UpcEndExclusive(void)
{}

static inline
void rted_staySafe(void)
{}

static inline
void rted_UpcAllInitWorkzone(void)
{}

static inline
void rted_sync_log(void)
{}

static inline
void rted_barrier(void)
{}

static inline
void rted_EnterSharedPtr(rted_Address addr)
{
  UNUSEDARG(addr);
}

static inline
void rted_ExitSharedPtr(rted_Address addr)
{
  UNUSEDARG(addr);
}

#endif /* WITH_UPC */

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* RTED_SYNC_H */
