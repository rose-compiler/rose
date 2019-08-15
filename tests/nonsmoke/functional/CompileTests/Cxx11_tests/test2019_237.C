namespace A
   {
     int foo();

     struct B
        {
       // The name of a function first introduced in a friend declaration is in the 
       // scope of the first nonclass scope that contains the enclosing class. 
       // If you declare a friend in a local class, and the friend name is unqualified, 
       // the compiler looks for the name only within the innermost enclosing nonclass 
       // scope. You must declare a function before declaring it as a friend of a local 
       // scope class. You do not have to do so with classes.
       // So the scope of foo is in namespace A and it should be refered to as A::foo() 
       // outside of the namespace A.
          friend int foo()
             {
               return 2;
             }
        };
   }

void foobar()
   {
     A::foo();
   }
