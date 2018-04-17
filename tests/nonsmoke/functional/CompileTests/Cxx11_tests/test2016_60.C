
#include <typeinfo>

  struct _Sp_make_shared_tag { };
  struct _Allo { };
  enum _Lock_policy { _S_single, _S_mutex, _S_atomic };

  template<typename _Tp, _Lock_policy _Lp>
  class __shared_ptr
    {
     template<typename _Alloc, typename... _Args>
     __shared_ptr(_Sp_make_shared_tag __tag, const _Alloc& __a, _Args&&... __args)
        {
          typeid(__tag);
        }
    };

