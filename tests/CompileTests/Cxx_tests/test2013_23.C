// This test code demonstrates the requirement for the "mutable" keyword.

typedef int _Atomic_word;

static inline void __atomic_add_dispatch(_Atomic_word* __mem) {}

class locale
   {
     public:
          class facet;
          friend class facet;
   };

class locale::facet
   {
     private:
          friend class locale;

       // Note that "mutable" keyword is required and dropped by ROSE (which is the demonstrated bug in this code).
          mutable _Atomic_word _M_refcount;

     private:
          void _M_add_reference() const // throw()
             {
            // __gnu_cxx::__atomic_add_dispatch(&_M_refcount, 1); 
            // __gnu_cxx::__atomic_add_dispatch(&(this) -> _M_refcount);
               __atomic_add_dispatch(&(this) -> _M_refcount);
             }
   };

