// #include <memory>
// #include <backward/auto_ptr.h>

// #include <bits/c++config.h>
// #include <debug/debug.h>

// _GLIBCXX_BEGIN_NAMESPACE(std)
namespace std {

  template<typename _Tp1>
    struct auto_ptr_ref
    {
      _Tp1* _M_ptr;
      
      explicit
      auto_ptr_ref(_Tp1* __p): _M_ptr(__p) { }
  }; // _GLIBCXX_DEPRECATED_ATTR;


  template<typename _Tp>
  class auto_ptr
     {
       private:
//            _Tp* _M_ptr;
      
       public:
            typedef _Tp element_type;

            explicit auto_ptr(element_type* __p = 0) throw() /* : _M_ptr(__p) */ { }

   // Commenting this out allows the code to compile with ROSE.
        auto_ptr(auto_ptr& __a) throw() /* : _M_ptr(__a.release()) */ { }

//     template<typename _Tp1> auto_ptr(auto_ptr<_Tp1>& __a) throw() : _M_ptr(__a.release()) { }

//      auto_ptr& operator=(auto_ptr& __a) throw() { return *this; }

//      template<typename _Tp1> auto_ptr& operator=(auto_ptr<_Tp1>& __a) throw() { return *this; }

//       ~auto_ptr() {} // delete _M_ptr; }

//      element_type& operator*() const throw() { return *_M_ptr; }

//      element_type* operator->() const throw() { return _M_ptr; }
      
#if 0
      element_type* get() const throw() { return _M_ptr; }
#endif
      
      element_type* release() throw()
      {
        element_type* __tmp; // = _M_ptr;
        return __tmp;
      }
      
//    void reset(element_type* __p = 0) throw() {}
      
      auto_ptr(auto_ptr_ref<element_type> __ref) throw() /* : _M_ptr(__ref._M_ptr) */ { }
      
//    auto_ptr& operator=(auto_ptr_ref<element_type> __ref) throw() { return *this; }
      
      template<typename _Tp1> operator auto_ptr_ref<_Tp1>() throw() { return auto_ptr_ref<_Tp1>(this->release()); }

//    template<typename _Tp1> operator auto_ptr<_Tp1>() throw() { return auto_ptr<_Tp1>(this->release()); }
  };

  template<>
  class auto_ptr<void>
     {
       public:
            typedef void element_type;
     };
}


class MatOpCommand {};

class CleanMatOpCommand : public MatOpCommand 
   {
     public:
       // explicit CleanMatOpCommand(double min_volume_fraction);
       // CleanMatOpCommand(double min_volume_fraction);
       // explicit CleanMatOpCommand(double min_volume_fraction) : m_min_volume_fraction(min_volume_fraction) {}
       // CleanMatOpCommand(int min_volume_fraction);
          CleanMatOpCommand(int min_volume_fraction) {}
   };

std::auto_ptr<MatOpCommand> parseCleanCommand()
   {
     double min_volume_fraction;
  // Bug: this unparses to be:
  // return ((std::auto_ptr< MatOpCommand > ::auto_ptr((new CleanMatOpCommand (42))) . operator std::auto_ptr_ref<MatOpCommand>()));
  // and should be:
  // return std::auto_ptr<MatOpCommand>(new CleanMatOpCommand(42));

  // return std::auto_ptr<MatOpCommand>(new CleanMatOpCommand(min_volume_fraction));
     return std::auto_ptr<MatOpCommand>(new CleanMatOpCommand(42));
   }
