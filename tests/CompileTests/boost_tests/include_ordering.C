
#include <boost/shared_ptr.hpp>

// boost 1.53.0 and later moved to dynamic_pointer_cast from dynamic_cast_tag
#include "boost/version.hpp"

// #if 1
#if BOOST_VERSION > 105200
   #define SHARED_PTR_CAST(TYPE,VAR) boost::dynamic_pointer_cast<TYPE>(VAR)
#else
   #define SHARED_PTR_CAST(TYPE,VAR) VAR, boost::detail::dynamic_cast_tag()
#endif

#if BOOST_VERSION > 105200
   #warning "using later version of boost"
#else
   #warning "using earlier version of boost"
#endif

struct TTT {};


template<class T> class df_shared_ptr: public boost::shared_ptr<T>
   {
     public:
          df_shared_ptr() {}

          template<class Y>
          explicit df_shared_ptr(Y * p)
             {
               boost::shared_ptr<T>::reset(p);
             }

       // template<class Y> df_shared_ptr(df_shared_ptr<Y> const & r) : boost::shared_ptr<T>(r,boost::detail::dynamic_cast_tag()) {}
          template<class Y> df_shared_ptr(df_shared_ptr<Y> const & r) : boost::shared_ptr<T>(SHARED_PTR_CAST(df_shared_ptr<Y>,r)) {}

   };


void foobar()
   {
     df_shared_ptr<TTT> tt;
   }
