// template <typename T>
class vector
   {
     public:
          double & operator[](int n);
   };

int z = 0;

// void foobar( vector<double> & data )
void foobar( vector & data )
   {
     data[z] = 0.0;
   }
