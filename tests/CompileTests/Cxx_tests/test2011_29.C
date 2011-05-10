namespace X
   {
#if 0
     class bad_alloc // : public exception 
        {
          public:
               bad_alloc() throw() {}
        };
#endif
     struct nothrow_t { };

     typedef unsigned long size_t;

  // extern const nothrow_t nothrow;
   }

// void operator delete[](void*) throw();
// void* operator new(X::size_t, const X::nothrow_t&) throw();
// void* operator new[](X::size_t, const X::nothrow_t&) throw();
// void operator delete(void*, const X::nothrow_t&) throw();
// void operator delete[](void*, const X::nothrow_t&) throw();


void* foo(X::size_t, const X::nothrow_t&) throw();
