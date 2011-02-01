

//
// creates the interfaces to support RTED on UPC
//
// On on UPC code, the interfaces decay to static inline functions

#ifndef _PARALLEL_RTS_H
#define _PARALLEL_RTS_H

#include "CppRuntimeSystem/rted_iface_structs.h"
#include "CppRuntimeSystem/ptrops.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef WITH_UPC

/// \brief polls incoming message buffer
void rted_ProcessMsg();

/// \brief sends a free message to all other processes
void snd_FreeMemory(rted_Address addr, rted_AllocKind freeKind, rted_SourceInfo si);

/// \brief shares information about non-local heap allocations
void snd_CreateHeapPtr(rted_TypeDesc, rted_Address, size_t, size_t, rted_AllocKind, const char*, rted_SourceInfo);

/// \brief shares information about variable initializations
void snd_InitVariable(rted_TypeDesc, rted_Address, size_t, int, rted_AllocKind, const char*, rted_SourceInfo);


/// \brief initializes the runtime system
void rted_UpcInitialize( );

#else /* WITH_UPC */

// when we do not use UPC, we compile the runtime system with empty
//   implementations.
static inline
void rted_ProcessMsg()
{}

static inline
void msg_FreeMemory( rted_Address r, rted_AllocKind a, rted_SourceInfo s)
{}

static inline
void snd_CreateHeapPtr(rted_TypeDesc td, rted_Address ad, size_t sz, size_t mallocsz, rted_AllocKind ak, const char* cn, rted_SourceInfo si)
{}

static inline
void rted_UpcInitialize()
{}

static inline
void snd_InitVariable(rted_TypeDesc td, rted_Address address, size_t size, int pointer_changed, rted_AllocKind allocKind, const char* class_name, rted_SourceInfo si)
{}

#endif /* WITH_UPC */


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _PARALLEL_RTS_H */
