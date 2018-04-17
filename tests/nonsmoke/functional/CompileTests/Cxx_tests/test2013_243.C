#include <bits/cpp_type_traits.h>

namespace std {
     typedef unsigned long size_t;

     typedef signed long nothrow_t;
}


void* operator new(std::size_t) throw ();
void* operator new[](std::size_t) throw ();
void operator delete(void*) throw();
void operator delete[](void*) throw();

#if 1
void* operator new(std::size_t, const std::nothrow_t&) throw();
void* operator new[](std::size_t, const std::nothrow_t&) throw();
void operator delete(void*, const std::nothrow_t&) throw();
void operator delete[](void*, const std::nothrow_t&) throw();
#endif

// Default placement versions of operator new.
inline void* operator new(std::size_t, void* __p) throw() { return __p; }
inline void* operator new[](std::size_t, void* __p) throw() { return __p; }

// Default placement versions of operator delete.
inline void  operator delete  (void*, void*) throw() { }
inline void  operator delete[](void*, void*) throw() { }


#if 0
// void* operator new (void* placement, size_t s)
void* operator new (size_t s, void* placement)
   {
     return 0L;
   }
#endif

namespace std {

template<typename _T1, typename _T2>
inline void
_Construct(_T1* __p, const _T2& __value)
   {
  // _GLIBCXX_RESOLVE_LIB_DEFECTS
  // 402. wrong new expression in [some_]allocator::construct
     ::new(static_cast<void*>(__p)) _T1(__value);
   }
}

void foo()
   {
     int* x_ptr;
     int y;
     std::_Construct<int,int>(x_ptr,y);

     int* a = ::new(static_cast<void*>(x_ptr)) int(y);
   }

