namespace internal {

// Note that EDG does not require this to be declared in the namespace (but gnu g++ (version 4.4.7) does.
struct X
   {
     void foo();
   };

#if 0
namespace 
   {
     int variable;
   }  // namespace
#else
int variable;
#endif

void X::foo() 
   {
#if 0
     int extension = variable;
#else
     int extension = internal::variable;
#endif
   }

}  // namespace internal

