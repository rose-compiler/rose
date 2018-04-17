
namespace std
   {
     template<typename _InputIterator>
     inline unsigned long*
     distance(_InputIterator __first, _InputIterator __last)
        {
          return 0;
        }
   }

namespace std
   {
     template<typename _ForwardIterator, typename _Tp>
     _ForwardIterator
     lower_bound(_ForwardIterator __first, _ForwardIterator __last,  const _Tp& __val)
        {
          std::distance(__first, __last);

          return __first;
        }
   }

namespace __gnu_cxx
   {
     inline unsigned long __stl_next_prime(unsigned long __n)
        {
          const unsigned long* __first;
          const unsigned long* __last;
          std::lower_bound(__first, __last, __n);

          return 0UL;
        }
   }

