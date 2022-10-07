
#pragma once

#include <cstdlib>
#include <cstdio>
#include <new>

extern bool TEST_IS_ACTIVE;

void* operator new(std::size_t n) throw(std::bad_alloc)
{
  if (TEST_IS_ACTIVE) printf("(g");
  return malloc(n);
}

void operator delete(void* p) throw()
{
  if (TEST_IS_ACTIVE) printf("g)");
  free(p);
}

void* operator new[](std::size_t n) throw(std::bad_alloc)
{
  if (TEST_IS_ACTIVE) printf("[g");
  return malloc(n);
}

void operator delete[](void *p) throw()
{
  if (TEST_IS_ACTIVE) printf("g]");
  free(p);
}
