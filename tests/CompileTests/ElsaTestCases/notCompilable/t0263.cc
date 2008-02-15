// t0263.cc
// inner union that is referenced twice inside the template

struct C {
  struct C *p;
};

  template<bool __threads, int __inst>
    class __default_alloc_template
    {
      union _Obj
      {
        union _Obj* _M_free_list_link;
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


