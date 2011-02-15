

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
void rted_ProcessMsg(void);

/// \brief sends a free message to all other processes
void snd_FreeMemory(rted_Address addr, rted_AllocKind freeKind, rted_SourceInfo si);

/// \brief shares information about non-local heap allocations
void snd_CreateHeapPtr(rted_TypeDesc, rted_Address, rted_Address, rted_AddressDesc, size_t, size_t, rted_AllocKind, const char*, rted_SourceInfo);

/// \brief shares information about variable initializations
void snd_InitVariable(rted_TypeDesc, rted_Address, rted_Address, rted_AddressDesc, size_t, int, const char*, rted_SourceInfo);

/// \brief shares information about pointer movements
void snd_MovePointer(rted_TypeDesc, rted_Address, rted_Address, rted_AddressDesc, const char*, rted_SourceInfo);

/// \brief initializes the runtime system
void rted_UpcAllInitialize(void);

/// \brief releases the heap read lock
void rted_UpcEnterWorkzone(void);

/// \brief acquires the heap read lock
void rted_UpcExitWorkzone(void);

/// \brief starts an exclusive heap operation (i.e., upc_free)
void rted_UpcBeginExclusive(void);

/// \brief ends an exclusive heap operation (i.e., upc_free)
///        see also rted_beginUpcHeapExclusive
void rted_UpcEndExclusive(void);

#else /* WITH_UPC */

// when we do not use UPC, we compile the runtime system with empty
//   implementations.
static inline
void rted_ProcessMsg(void)
{}

static inline
void msg_FreeMemory(rted_Address r, rted_AllocKind a, rted_SourceInfo s)
{}

static inline
void snd_CreateHeapPtr(rted_TypeDesc td, rted_Address ad, rted_Address heap_addr, rted_AddressDesc heap_desc, size_t sz, size_t mallocsz, rted_AllocKind ak, const char* cn, rted_SourceInfo si)
{}

static inline
void snd_InitVariable(rted_TypeDesc td, rted_Address address, rted_Address heap_addr, rted_AddressDesc heap_desc, size_t sz, int pointer_moved, const char* classname, rted_SourceInfo si);
{}

static inline
void snd_MovePointer(rted_TypeDesc td, rted_Address addr, rted_Address heap_addr, rted_AddressDesc heap_desc, const char* class_name, rted_SourceInfo si)
{}

static inline
void rted_UpcAllInitialize(void)
{}

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

#endif /* WITH_UPC */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _PARALLEL_RTS_H */
