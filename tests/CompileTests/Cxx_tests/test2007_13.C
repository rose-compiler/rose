

// This case demonstrates a name qualificiation bug in ROSE (over use of "::" global qualifier)
// This case fails on GNU 3.3.x and 3.4.x compilers
class Y
   {
   };

class X
   {
     static Y Yobject0;
     static Y Yobject1;
   };

// This is the initialization of X::Yobject0
// This is unparsed to: "class ::Y ::X::Yobject;" where the error message is that "class Y::X' has not been declared"
Y X::Yobject0;

namespace X_namespace
   {
     class Y
        {
        };

     class X
        {
          static X_namespace::Y Yobject1;
          static Y Yobject2;
          static Y Yobject3;
        };

  // This is unparsed to: "class ::Y ::X::Yobject;" where the error message is that "class Y::X' has not been declared"
     X_namespace::Y X_namespace::X::Yobject1;

  // This is inside of the X_namespace and initializes X_namespace::X::Yobject2
     Y X::Yobject2;

  // This appears to not be allowed (illeagal C++)!
  // ::Y ::X::Yobject0;
   }

// This is the initialization of X::Yobject1
// This used to be unparsed to: "class ::Y ::X::Yobject;" where the error message is that "class Y::X' has not been declared"
Y X::Yobject1;

// This is outside of the X_namespace and initializes X_namespace::X::Yobject3
X_namespace::Y X_namespace::X::Yobject3;
