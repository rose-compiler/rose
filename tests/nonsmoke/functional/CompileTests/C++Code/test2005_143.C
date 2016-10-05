// Error: original code:
//    Vector unit_x = x / ( x.magnitude() + epsilon() );
// is unparsed as:
//    Vector unit_x = x / x.magnitude() + epsilon();


class myVector
   {
     public:
          double norm() const;
          myVector operator/( double x) const;

          myVector operator*(const myVector & x) const;
          myVector operator/(const myVector & x) const;
          myVector operator+(const myVector & x) const;
          myVector operator-(const myVector & x) const;

          myVector();

       // Presence of explicit copy constructor causes code generation to be "value = b-c.norm();"
          myVector (const myVector &x);

       // An explicit operator= appears to have no effect!
          myVector operator= (const class vector &x) const;
   };

double min (double x, double y);

void foo()
   {
     myVector a;

  // Problem code
     myVector b = a / (a.norm() + 1);

  // b = (a + b) / a;
  // b = a / (a + b);

     int offset;
     
   }
