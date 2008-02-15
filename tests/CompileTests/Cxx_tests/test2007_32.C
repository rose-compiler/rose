/*
Dan,

In trying to integrate the outliner and its tests, I ran into a possible problem with name qualification unparsing. I can't remember if this is a "known" problem or not.

Consider the following source file:

 namespace N
 {
   void foo (void)
   {
     extern char z;
     z = '5';
   }
 }

g++ accepts this input, and when I inspect the symbols inside the object file, the reference to "z" appears essentially as a reference to N::z (i.e., the mangled name that appears in the object file corresponds to "char N::z").

On the above input, the ROSE identity translator produces:

 namespace N
 {
   void foo()
   {
     extern char N::z;
     ::N::z = '5';
   }
 }

The qualification is logically correct, but g++ does not accept this unparsed output, reporting the error,

 rose_example5.cc: In function void N::foo():
 rose_example5.cc:7: error: z is not a member of N

So, this leaves me wondering: What is the rule that says 'z' should not be explicitly qualified in this case?

--rich 
*/

namespace N
   {
     void foo (void)
        {
          extern char z;
          z = '5';
        }
   }
