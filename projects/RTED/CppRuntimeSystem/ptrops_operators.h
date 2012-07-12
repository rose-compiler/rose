
/// \file  ptrops_operators.h
/// \brief provides C++ operators and other operations on RTED's address
///        abstraction (rted_Address)
/// \email peter.pirkelbauer@llnl.gov

#ifndef _PTROPS_OPERATORS_H
#define _PTROPS_OPERATORS_H

#include <cmath>
#include <ostream>
#include <iostream>

#include "ptrops.h"
#include "rted_typedefs.h"


// from ez/util.hpp
namespace ez
{
  template <class T>
  static inline
  void unused(const T&) {}
}


inline
std::ostream& operator<<(std::ostream& s, const Address& obj)
{
  const void* addr = obj.local;

  s << addr;

#ifdef WITH_UPC
  s << " @" << obj.thread_id;

  if (rted_isLocal(obj)) s << " (local)";
#endif /* WITH_UPC */

  return s;
}

inline
bool operator<(const Address& lhs, const Address& rhs)
{
  return (  lhs.local < rhs.local
#if WITH_UPC
         || (lhs.local == rhs.local && lhs.thread_id < rhs.thread_id)
#endif /* WITH_UPC */
         );
}

inline
bool operator<=(const Address& lhs, const Address& rhs)
{
  return !(rhs < lhs);
}


inline
bool operator==(const Address& lhs, const Address& rhs)
{
  return (  lhs.local == rhs.local
#if WITH_UPC
         && (  lhs.local == 0
            || lhs.thread_id == rhs.thread_id
            )
#endif /* WITH_UPC */
         );
}

inline
bool operator!=(const Address& lhs, const Address& rhs)
{
  return !(lhs == rhs);
}


/// \brief   implements Knuth's floor division
/// \details a better match for the two dimensional pointer arithmetic
///          i.e, sub can be implemented in terms of add
template <class FPT = double>
struct FloorDiv
{
  template <class T>
  static inline
  std::pair<T, T>
  div(const T& dividend, const T& divisor)
  {
    using std::floor;

    T q = T(floor( FPT(dividend) / FPT(divisor) ));
    T r = dividend - q * divisor;

    return std::pair<T, T>(q, r);
  }
};

static inline
std::ostream& operator<<(std::ostream& out, std::pair<long, long> p)
{
  return out << "(" << p.first << ", " << p.second << ")";
}

/// \brief   adds ofs bytes to an address
/// \details takes into account whether the memory is distributed. the argument
///          distributed needs to be false for local memory and shared
///          memory allocated with upc_alloc. true for other shared memory
///          (i.e., allocated on the stack, or heap with upc_global_alloc,
///          upc_all_alloc)
/// \todo    Also, we likely need to take the blocking factor into account.
static inline
Address add(Address l, long ofs, long blocksize)
{
  ez::unused(blocksize);

#ifdef WITH_UPC
  typedef FloorDiv<> floordiv;

  // \todo distributed < 0
  if (blocksize > 0)
  {
    const std::pair<long, long> blocks = floordiv::div(ofs, blocksize);
    const long                  threadcount = rted_Threads();
    const long                  adjblocks = blocks.first + l.thread_id;
    const std::pair<long, long> threads = floordiv::div(adjblocks, threadcount);

#if EXTENDEDDBG
    std::cerr << "addr = " << l << std::endl
              << "ofs = " << ofs << std::endl
              << "bs = " << blocksize << std::endl
              << std::endl
              << "blocks = " << blocks << std::endl
              << "threads = " << threadcount << std::endl
              << "adjblocks = " << adjblocks << std::endl
              << "threads = " << threads << std::endl;
#endif /* EXTENDEDDBG */

    l.thread_id = threads.second;
    ofs = (threads.first * blocksize) + blocks.second;
  }
#endif /* WITH_UPC */

  l.local += ofs;
  return l;
}

/// \brief variant that adjusts the address for a specified phase within a block
///        before adding the offset to the address.
inline
Address add(Address l, long ofs, long blocksize, long phase)
{
   l.local -= phase;
   ofs += phase;

   return add(l, ofs, blocksize);
}

inline
Address nullAddr()
{
  return rted_Addr(NULL);
}

inline
bool isNullAddr(const Address& obj)
{
  return obj.local == NULL;
}

#if OBSOLETE_CODE
/// \brief   subtracts ofs bytes from the address
/// \details see comments on add
static inline
Address sub(Address l, long ofs, bool distributed, int blockofs)
{
  return add(l, -ofs, distributed);
}

/// \note see byte_offset in MemoryManager.cpp
long ofs(Address lhs, Address rhs, bool distributed);
#endif /* OBSOLETE_CODE */

#endif /* _PTROPS_OPERATORS_H */
