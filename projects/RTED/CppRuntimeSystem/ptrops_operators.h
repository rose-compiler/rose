
#ifndef _PTROPS_OPERATORS_H
#define _PTROPS_OPERATORS_H

#include <ostream>

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

inline
Address& operator+=(Address& lhs, long offset)
{
  lhs.local += offset;
  return lhs;
}

inline
Address operator+(const Address& lhs, long offset)
{
  Address tmp(lhs);

  tmp.local += offset;
  return tmp;
}

inline
long operator-(const Address& lhs, const Address& rhs)
{
  return lhs.local - rhs.local;
}

inline
Address operator-(const Address& lhs, long offset)
{
  return lhs + (-offset);
}

inline
std::ostream& operator<<(std::ostream& s, const Address& obj)
{
  const void* addr = obj.local;

  s << addr;

#if WITH_UPC
  s << " @" << obj.thread_id;

  if (rted_isLocal(obj)) s << " (local)";
#endif /* WITH_UPC */

  return s;
}

inline
bool isNullAddr(const Address& obj)
{
  return obj.local == NULL;
}

inline
Address nullAddr()
{
  Address obj;

  obj.local = NULL;
  return obj;
}



#endif /* _PTROPS_OPERATORS_H */
