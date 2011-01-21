
#ifndef _PTROPS_OPERATORS_H
#define _PTROPS_OPERATORS_H

#include <ostream>

#include "rted_typedefs.h"

inline
bool operator<(const Address& lhs, const Address& rhs)
{
  return lhs.local < rhs.local;
}

inline
bool operator<=(const Address& lhs, const Address& rhs)
{
  return lhs.local <= rhs.local;
}

inline
bool operator==(const Address& lhs, const Address& rhs)
{
  return lhs.local == rhs.local;
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

  return s << std::setfill('0') << std::setw(6) << std::hex << addr;
  // return s << obj.local;
}

template <class T>
inline
const T* point_to(const Address& addr)
{
  return reinterpret_cast<const T*>(addr.local);
}

inline
Address memAddr(size_t sysaddr)
{
  Address tmp;

  tmp.local = reinterpret_cast<char*>(sysaddr);
  return tmp;
}

template <class T>
inline
Address memAddr(const T* t)
{
  Address tmp;

  tmp.local = reinterpret_cast<const char*>(t);
  return tmp;
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
