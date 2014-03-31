
class X
   {
     public:
          template <typename T> void foo() {}
   };


// Template instantiation Directive for a member function
template void X::foo<int>();
