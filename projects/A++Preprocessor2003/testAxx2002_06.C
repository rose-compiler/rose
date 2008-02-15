class Range 
   {
     public:
          Range ( int base, int bound, int stride );
          Range operator+ ( int i );
          Range operator- ( int i );
   };

class doubleArray
   {
     public:
          doubleArray ( int i, int j );
          doubleArray & operator= ( const double x );
          doubleArray & operator= ( const doubleArray & X );
          friend doubleArray operator+ ( const doubleArray & X, const doubleArray & Y );
          doubleArray operator() ( const Range & I, const Range & J );
   };

int
main ()
   {
     doubleArray A(100,100);
     doubleArray B(100,100);
     Range I(1,98,1),J(1,98,1);
  // A(I,J) = B(I-1,J) + B(I+1,J) + B(I,J-1) + B(I,J+1);
     A = 0;
   }
