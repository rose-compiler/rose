
// foo ( struct { int x; } X );


// Any transformation to normalize the SAGE representation of structures and introduce
// two statements in place of an autonomous declaration hidden in a variable declaration 
// can not be applied to the initializer in a "for" loop.
int main()
   {
  // complex structures may be defined within the initialization of variables in a for loop
     for ( struct Xtag { int x; bool operator==(int){ for(Xtag x; x == 0;) x.x=1; return false;} } X; X == 0;) 
        { }
     return 0;
   }
