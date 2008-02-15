
// Code such as:   length = min(length,(pos-fpos).magnitude());
// is unparsed as: length = min(length,(pos-fpos.magnitude()));

class vector
   {
     public:
          double norm() const;
          vector operator- ( const vector & x ) const;
       // vector operator- () const;

          vector();

       // Presence of explicit copy constructor causes code generation to be "value = b-c.norm();"
          vector (const class vector &x);

       // An explicit operator= appears to have no effect!
       // vector operator= (const class vector &x) const;
   };

double min (double x, double y);

void foo()
   {
     double value;
     vector b;
     vector c;

  // Problem code
     value = (b-c).norm();

  // Another (more complex) example
     value = min(value,(b-c).norm());
   }


