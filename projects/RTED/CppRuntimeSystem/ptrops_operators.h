
#ifndef _PTROPS_OPERATORS_H
#define _PTROPS_OPERATORS_H

#include <cmath>
#include <ostream>

#include "ptrops.h"
#include "rted_typedefs.h"

inline
bool operator<(const Address& lhs, const Address& rhs)
{
  return (  lhs.local < rhs.local
#if WITH_RTED
         || (lhs.local == rhs.local && lhs.thread_id < rhs.thread_id)
#endif /* WITH_RTED */
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
#if WITH_RTED
         && lhs.thread_id == rhs.thread_id
#endif /* WITH_RTED */
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

/// \brief   adds ofs bytes to an address
/// \details takes into account whether the memory is distributed. the argument
///          distributed needs to be false for local memory and shared
///          memory allocated with upc_alloc. true for other shared memory
///          (i.e., allocated on the stack, or heap with upc_global_alloc,
///          upc_all_alloc)
/// \todo    Also, we likely need to take the blocking factor into account.
static inline
Address add(Address l, long ofs, bool distributed)
{
  typedef FloorDiv<> floordiv;

  if (distributed)
  {
    const long                  threadcount = rted_Threads();
    const long                  adjofs = l.thread_id + ofs;
    const std::pair<long, long> divres = floordiv::div(adjofs, threadcount);

    l.thread_id = divres.second;
    ofs = divres.first;
  }

  l.local += ofs;
  return l;
}

/*
/// \brief   subtracts ofs bytes from the address
/// \details see comments on add
static inline
Address sub(Address l, long ofs, bool distributed)
{
  return add(l, -ofs, distributed);
}

/// \brief   calculates the number of bytes between lhs and rhs
/// \details see comments on add
static inline
long diff(Address lhs, Address rhs, bool distributed)
{
  long ofs = lhs.local - rhs.local;

  if (distributed)
  {
    ofs *= rted_Threads();
    ofs += lhs.thread_id - rhs.thread_id;
  }

  return ofs;
}
*/

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
Address nullAddr()
{
  return rted_Addr(NULL);
}

inline
bool isNullAddr(const Address& obj)
{
  return obj.local == NULL;
}

#endif /* _PTROPS_OPERATORS_H */
