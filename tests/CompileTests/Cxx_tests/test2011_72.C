
// #include<string>
namespace A
   {
     class string {};
   }

class X
   {
     public:
          int A() const;

          A::string getValue() const
             { 
               return valueArray[int(A())];
             }

       // Note that this SgInitializedName does not have a valid parent pointer.
       // It was built when the "return valueArray[int(A())];" was built.
          static const int arraySize = 16;

          static const A::string valueArray[arraySize+10];
   };


#if 0
// This is not significantly different from the example above
// Build a static array of objects (using X)
class Y
   {
     public:
          X getValue() const
             { 
               return valueArray[0];
             }

       // This appears after its use in the getValue() function (above) and so it is not declared yet.
          static const X valueArray[2];
   };

// const X Y::valueArray[2] = {1,2};
#endif

