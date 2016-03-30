
class array
   {
     public:
          int* data;

       // default constructor
          array();

       // copy constructor
          array(const array & X);

       // Assignment operator
          array operator= (const array & X);

       // simple array operator
          friend array operator+ (const array & X, const array & Y);
   };
