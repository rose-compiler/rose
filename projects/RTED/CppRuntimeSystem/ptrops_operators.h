
#ifndef _PTROPS_OPERATORS_H
#define _PTROPS_OPERATORS_H

#include <ostream>

#include "ptrops.h"

inline
bool operator<(const MemoryAddress& lhs, const MemoryAddress& rhs)
{
  return lhs.local < rhs.local;
}

inline
bool operator<=(const MemoryAddress& lhs, const MemoryAddress& rhs)
{
  return lhs.local <= rhs.local;
}

inline
bool operator==(const MemoryAddress& lhs, const MemoryAddress& rhs)
{
  return lhs.local == rhs.local;
}

inline
bool operator!=(const MemoryAddress& lhs, const MemoryAddress& rhs)
{
  return !(lhs == rhs);
}

inline
MemoryAddress& operator+=(MemoryAddress& lhs, long offset)
{
  lhs.local += offset;
  return lhs;
}

inline
MemoryAddress operator+(const MemoryAddress& lhs, long offset)
{
  MemoryAddress tmp(lhs);

  tmp.local += offset;
  return tmp;
}

inline
long operator-(const MemoryAddress& lhs, const MemoryAddress& rhs)
{
  return lhs.local - rhs.local;
}

inline
MemoryAddress operator-(const MemoryAddress& lhs, long offset)
{
  return lhs + (-offset);
}

inline
std::ostream& operator<<(std::ostream& s, const MemoryAddress& obj)
{
  const void* addr = obj.local;

  return s << std::setfill('0') << std::setw(6) << std::hex << addr;
  // return s << obj.local;
}

template <class T>
inline
T* point_to(const MemoryAddress& addr)
{
  return reinterpret_cast<T*>(addr.local);
}

inline
MemoryAddress memAddr(size_t sysaddr)
{
  MemoryAddress tmp;

  tmp.local = reinterpret_cast<char*>(sysaddr);
  return tmp;
}

template <class T>
inline
MemoryAddress memAddr(T* t)
{
  MemoryAddress tmp;

  tmp.local = reinterpret_cast<char*>(t);
  return tmp;
}

template <class T>
inline
MemoryAddress memAddr(const T* t)
{
  MemoryAddress tmp;

  tmp.local = reinterpret_cast<char*>(const_cast<T*>(t));
  return tmp;
}

inline
bool isNullAddr(const MemoryAddress& obj)
{
  return obj.local == NULL;
}

inline
MemoryAddress nullAddr()
{
  MemoryAddress obj;

  obj.local = NULL;
  return obj;
}


#endif /* _PTROPS_OPERATORS_H */
