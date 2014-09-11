
namespace ABC
   {
  // Interestingly a value type can't have a class type.
  // class A {};
     typedef unsigned short A;
   }

template <ABC::A T>
class X
   {
     public:
       // This function declaration will be unparsed as a function
       // prototype in the generated templated class and the
       // defining declaration will be unparsed seperately.
          void free(X ptr) 
             {
               int i = T;
             }
   };

void foo()
   {
     const ABC::A abc_value = 7;

  // Note that this is unparsed as: "class X< 7 > a;" instead of "X<abc_value> a;" (not really a serious problem).
     X<abc_value> a;

     a.free(a);
   }
