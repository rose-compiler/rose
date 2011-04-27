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

#else /* WITH_UPC */

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

#endif /* WITH_UPC */

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* RTED_SYNC_H */
