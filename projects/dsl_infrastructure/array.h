#pragma DSL keyword
class array
   {
     public:
       // int* data;

       // Default constructor
          array();

       // Copy constructor
       // array(const array & X);

       // Assignment operator
          array operator= (const array & X);
          array operator= (int x);

       // Simple array arithmetic operators
       // friend array operator* (const array & X, const array & Y);
       // friend array operator/ (const array & X, const array & Y);
          friend array operator+ (const array & X, const array & Y);
       // friend array operator- (const array & X, const array & Y);

       // returns array of size set to zeros.
       // array zero(int size);

       // returns array of size set to ones.
       // array one (int size);
   };
