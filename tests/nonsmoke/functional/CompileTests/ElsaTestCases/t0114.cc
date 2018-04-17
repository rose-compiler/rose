// t0114.cc
// overloaded decls in template classes

// non-template class, as an initial control
class Foo {
      void addr(int &r) const;
      void addr(int const &r) const;
};

// template class
template <class T>
class Bar
{
    public:
      typedef T*         pointer;
      typedef const T*   const_pointer;

      typedef T&         reference;
      typedef const T&   const_reference;


      pointer
      address( reference r ) const;

      const_pointer
      address( const_reference r ) const;
      
      void addr(int &r) const;
      void addr(int const &r) const;
};

// force instantiation
typedef Bar<unsigned short> blah;
