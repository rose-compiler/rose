template<class T> int foo(T t);

struct A 
// class A 
   {
  // Member functions and data members and friend functions can be private in structs as well as classes.
     private:
          friend int foo<>(int);
   };
