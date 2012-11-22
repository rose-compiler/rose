template <typename T> class X
   {
     public:
          X ();
       // X ( const X & a );
          X ( X & a );
   };

void foo ()
   {
     X<int> xObject;

  // Unparses as:   class X< int  > *ptr = new X< int  > (1UL);
  // Should be: X<int>* ptr = new X<int>( (X<int> &) xObject );
     X<int>* ptr = new X<int>( (X<int> &) xObject );
   }

