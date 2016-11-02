// This test code tested the use of a template class and member function definition within a template class.
// However, later in debugging the use of GNU extensions in EDG and ROSE it became important in identifying
// a bug in EDg which causes the first declaration after 
//      either "void __builtin_va_start(__builtin_va_list &, void*);" or
//      "void va_start(__builtin_va_list &, void*);"
// to be eaten (ignored).  This is a very strange bug in EDG, or so it appears.

// Providing a declaration that can be ignored allows this code to compile if the 
// header file (rose_edg_macros_and_functions_required_for_gnu.h) is not fixed up
// with a declaration for EDG to ignore.  Only one declaration will be ignored!
// int x;
// class A {};
// class B {};

template<class T>
class TestClassArgument
   {
     public:
          T xyz;
          T foo();
   };

#if 1
template<class T>
T TestClassArgument<T>::foo ()
   {
     return xyz;
   }

#endif

#if 0
int main()
   {
     TestClassArgument<A> a;

     return 0;
   }
#endif
