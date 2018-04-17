

// Things that should work in C++, gcc and C99
// Examples from: http://gcc.gnu.org/onlinedocs/gcc-3.2.2/gcc/Compound-Literals.html#Compound%20Literals

void foobar()
   {
     struct foo {int a; char b[2];} structure;

     int x = 1, y = 2;

  // Here is an example of constructing a struct foo with a compound literal:
  // structure = ((struct foo) {x + y, 'a', 0});
     structure = ((struct foo) {x, 'a', 0});

  // This is equivalent to writing the following:
     {
       struct foo temp = {x + y, 'a', 0};
       structure = temp;
     }

#if 0
  // DQ (11/29/2005): Not yet tested.

  // You can also construct an array. If all the elements of the compound literal are 
  // (made up of) simple constant expressions, suitable for use in initializers of 
  // objects of static storage duration, then the compound literal can be coerced to 
  // a pointer to its first element and used in such an initializer, as shown here:
     char **foo = (char *[]) { "x", "y", "z" };

  // Compound literals for scalar types and union types are is also allowed, but then 
  // the compound literal is equivalent to a cast.

  // As a GNU extension, GCC allows initialization of objects with static storage duration
  // by compound literals (which is not possible in ISO C99, because the initializer is not
  // a constant). It is handled as if the object was initialized only with the bracket
  // enclosed list if compound literal's and object types match. The initializer list of
  // the compound literal must be constant. If the object being initialized has array type
  // of unknown size, the size is determined by compound literal size.

     static struct foo x = (struct foo) {1, 'a', 'b'};
     static int y[] = (int []) {1, 2, 3};
     static int z[] = (int [3]) {1};

  // The above lines are equivalent to the following:

     static struct foo x = {1, 'a', 'b'};
     static int y[] = {1, 2, 3};
     static int z[] = {1, 0, 0};
#endif
   }
