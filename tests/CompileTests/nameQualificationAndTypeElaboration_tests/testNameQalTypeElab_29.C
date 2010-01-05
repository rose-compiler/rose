// number #29
#define STORAGE
class Y
   {
     public:
       // This is allowed by GNU but not by EDG
       // static const double pi = 3.141592653589793238462643383279; // Pi to 30 places

       // This is allowed by EDG, but not by g++ (g++ needs constant to be static)
       // const double pi = 3.141592653589793238462643383279; // Pi to 30 places

       // Code that will compile with EDG
       // const double pi = 3.141592653589793238462643383279; // Pi to 30 places
       // Code that we should generate so that we can compile with g++
       // static const double pi = 3.141592653589793238462643383279; // Pi to 30 places

          int findme;

          static const double static_pi;
          STORAGE const double pi = 3.141592653589793238462643383279; // Pi to 30 places
   };

const double Y::static_pi = 3.14;
