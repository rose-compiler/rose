// This test code demonstrates an issue that is too complex for the existing infrastructure.
// The mechanism for storing name qualification will have to be extended to support this
// in the future.
class A 
   {
     static void func(int) {}
     public:
          template<void (*f)(int) > class int_temp{};
          int_temp<func>* func2();
   };

// BUG: template argument "&A::func" is unparsed as "func"
// and thus unparsed as: A::int_temp< func > *A::func2()
// The return type is shared, and so we need to support 
// name qualification on an arbitrary numbr of template 
// arguments in the shared return type.
A::int_temp<&A::func>* A::func2()
   {
     return 0L;
   }

