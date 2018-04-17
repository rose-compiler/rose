// t0264.cc
// an error that at one time caused a segfault

  template<bool __threads, int __inst>
    class __default_alloc_template
    {
      struct _Obj
      {
        //ERROR(1): union _Obj* _M_free_list_link;    // wrong keyword
        char _M_client_data[1];
      };


      static void
      deallocate(void* __p, int __n)
      {      }
    };

  typedef __default_alloc_template<true,0> __alloc;
  
  void foo()
  {
    __alloc::deallocate(0,0);
  }


