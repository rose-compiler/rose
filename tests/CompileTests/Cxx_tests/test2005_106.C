// This test code generates a warning which should likely be eliminated within the 
// EDG/Sage III translation.  There is a cast that in EDG is not represented within
// the Sage III AST, I don't think this is a problem (the way that the other implicit
// cast was a problem in test2005_37.C.

class X
   {
     public:
          char* array[10];
       // void foo ( const char* c ) const;
          void foo ( char* c );
   };

int main()
   {
     X x;

  // Generates warning: "Warning: compiler generated cast not explicit in Sage translation (skipped)"
     x.foo(x.array[0]);

  // This also generates the same warning (which I think can be ignored)
     char* ptr = x.array[0];
     x.foo(ptr);
   }
