#include <exception>
#include <typeinfo>

namespace boost
   {
#if 0
     void throw_exception(std::exception const & e); // user defined
#else
     template<class E> void throw_exception(E const & e)
        {
          throw e;
        }
#endif
   } // namespace boost

void foo()
   {
     boost::throw_exception(std::bad_cast());
   }
