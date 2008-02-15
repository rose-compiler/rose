
#if 0
// This case demonstrates a name qualificiation bug in ROSE (over use of "::" global qualifier)
// This case fails on GNU 3.3.x and 3.4.x compilers
class Y
   {
   };

class X
   {
     static Y Yobject;
   };

// This is unparsed to: "class ::Y ::X::Yobject;" where the error message is that "class Y::X' has not been declared"
Y X::Yobject;
#endif


#if ( ( (__GNUC__ == 3) && (__GNUC_MINOR__ == 4) ) || (__GNUC__ == 4) )
// This variation works fine on 3.4.6 (64-bit), but fails 3.3.2 (32-bit)
class A
   {
     class B
        {
          static A Xobject;
        };
   };

// This unparses to: "class ::A ::A::B::Xobject;"
A A::B::Xobject;
#endif

#if ( ( (__GNUC__ == 3) && (__GNUC_MINOR__ == 4) ) || (__GNUC__ == 4) )
// This case appears to work fine on 3.4.6 (64-bit), but fails 3.3.2 (32-bit) (just adding the class "X" as a nested class in class "Y")
// This has to be tested on the 4.x compilers
class Y
   {
     class X
        {
       // static Y Yobject;
        };
   };

class X
   {
     static Y Yobject;
   };

// This is unparsed to: "class ::Y ::X::Yobject;" where the error message is that "class Y::X' has not been declared"
Y X::Yobject;
#endif

