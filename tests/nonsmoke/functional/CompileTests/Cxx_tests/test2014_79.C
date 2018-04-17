       
typedef long int ptrdiff_t;
typedef long unsigned int size_t;

namespace std __attribute__ ((__visibility__ ("default"))) {
  using ::ptrdiff_t;
  using ::size_t;
}


namespace __gnu_cxx __attribute__ ((__visibility__ ("default"))) {
  using std::size_t;
  using std::ptrdiff_t;
  template<typename _Tp>
    class new_allocator
    {
    public:
      typedef _Tp* pointer;

      template<typename _Tp1>
        struct rebind
        { typedef new_allocator<_Tp1> other; };

    };

}


namespace std __attribute__ ((__visibility__ ("default"))) {

  template<typename _Tp>
     class allocator : public __gnu_cxx::new_allocator<_Tp>
    {
    };

}


namespace __gnu_cxx __attribute__ ((__visibility__ ("default"))) {
  enum _Lock_policy { _S_single, _S_mutex, _S_atomic };
  static const _Lock_policy __default_lock_policy =  _S_atomic;
}


namespace std {
namespace tr1 {

  using __gnu_cxx::_Lock_policy;
  using __gnu_cxx::__default_lock_policy;

}
}

namespace std
{
namespace tr1
{

  template<typename _Tp, _Lock_policy _Lp = __default_lock_policy> class __shared_ptr;

  template<typename _Tp, _Lock_policy _Lp>
    class __shared_ptr
    {
    private:
      template<typename _Tp1>
        friend inline bool
        operator==(const __shared_ptr& __a, const __shared_ptr<_Tp1, _Lp>& __b)
        { 
          return true;
        }


      _Tp* _M_ptr;
    };


  template<typename _Tp>
    class shared_ptr
    : public __shared_ptr<_Tp>
    {
    };
}
}

namespace scallop
{
   using std::tr1::shared_ptr;
}
       

namespace std __attribute__ ((__visibility__ ("default"))) {

  template<typename _Tp, typename _Alloc>
    struct _Vector_base
    {
      typedef typename _Alloc::template rebind<_Tp>::other _Tp_alloc_type;
      struct _Vector_impl
      : public _Tp_alloc_type
      {
         typename _Tp_alloc_type::pointer _M_finish;
      };
    public:

    public:
      _Vector_impl _M_impl;
    };

template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
   class vector : protected _Vector_base<_Tp, _Alloc>
    {
      public:
           typedef _Tp value_type;

      public:
         void push_back(const value_type& __x)
            {
               ++this->_M_impl._M_finish;
            }

    };
}


// namespace std __attribute__ ((__visibility__ ("default"))) {}


namespace scallop {
class MessagePackage;
class MessageProcessor;
namespace common {
class MessageProcessorData {
private:
public:
    void addMessage(shared_ptr<MessagePackage const> const& message)
       {
         messages.push_back(message);
       }
private:
    std::vector<shared_ptr<MessagePackage const> > messages;
};
}
}
