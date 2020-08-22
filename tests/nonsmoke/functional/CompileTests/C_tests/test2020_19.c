void foo()
   {
  // if ( ( ( union { int v; } ) {} ).v || ( ( union { int v; } ) {} ).v );
     if ( ( ( union { int v; } ) {} ).v || ( ( union { int v; } ) {} ).v )
       {
       }
   }
