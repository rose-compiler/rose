namespace N
   {
     enum class color : long;
     struct A 
        {
          enum class shape;
        };

     int foo(color c);
     int foo(A::shape s);
   }

enum class N::color : long { R, O, Y };
enum class N::A::shape { circle, rectangle, triangle };

void foobar()
   {
     N::foo(N::color::R);
     N::foo(N::A::shape::circle);
   }


