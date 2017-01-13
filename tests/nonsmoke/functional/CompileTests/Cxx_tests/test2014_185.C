void foo();

namespace internal {


#if 0
namespace 
   {
     int variable;
   }  // namespace
#else
int variable;
#endif

void foo() 
   {
     int extension = variable;
   }

}  // namespace internal

