// Error: original code:
//    Vector unit_x = x / ( x.magnitude() + epsilon() );
// is unparsed as:
//    Vector unit_x = x / x.magnitude() + epsilon();


class myVector
   {
     public:
          double norm() const;
          myVector operator/( double x) const;
          myVector();
   };

// double min (double x, double y);

void foo()
   {
     myVector a;

  // Problem code (parenthesis are required for "a.norm() + 1".
     myVector b = a / (a.norm() + 1);
   }
