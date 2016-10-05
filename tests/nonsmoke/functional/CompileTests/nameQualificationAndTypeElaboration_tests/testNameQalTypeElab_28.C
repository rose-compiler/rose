// number #28
class X
   {
     public:
       // Notice that we can initialize static constants within the class!
          static const int    maxIntValue    = 3;
          static const long   maxLongValue   = 3;
          static const char   maxCharValue   = 'Z';

       // Not allowed for float, double, or pointers (interesting!)
       // static const float  maxFloatValue  = 3.0;
       // static const double maxDoubleValue = 3.0;
       // static const size_t maxSizeTValue = 3;
       // static const int   *maxIntPointerValue = 0L;

       // types of data members that can't be initalized in-class
          static const double pi;
          const int tuncatedPi;
          const double e;

       // this is the only way to initalize a non-static const variable (integer or double)
          X(): tuncatedPi(3), e(2.71) {}
   };

// This is the only way to initialize a static const data member which is non-integer based
const double X::pi = 3.14;
