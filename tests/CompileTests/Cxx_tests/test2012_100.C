// This example shows how we can have multiple non-defining member functions
// test2005_121.C shows a different variation using templates.

// class X;
// void X::foobar();

class X
   {
     public:
          void foobar();
   };

// These can't be redeclared outside the class as they can be with functions!
// void X::foobar();

// void X::foobar() {}

// These can't be redeclared outside the class as they can be with functions!
// void X::foobar();

class Y
   {
     friend void X::foobar();
     public:
//          void foobar();
   };


