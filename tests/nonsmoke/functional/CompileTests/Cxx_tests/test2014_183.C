namespace internal {

// Redundant nested namespace causes a problem in ROSE...
namespace internal {}

struct X
   {
     void foo();
   };

namespace 
   {
     int variable;
   }  // namespace

void X::foo() 
   {
     int extension = variable;
   }

}  // namespace internal

