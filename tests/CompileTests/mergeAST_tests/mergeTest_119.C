// Example demonstrating that the "using A::B::b;" does not directly reference the SgInitializedName.
namespace A
   {
     namespace B
        {
          int b;
        };
   };

void
foo()
   {
     using A::B::b;
     b = 0;
   }
