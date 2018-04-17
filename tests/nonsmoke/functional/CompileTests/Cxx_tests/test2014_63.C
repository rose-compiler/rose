namespace test 
   {
     template<typename T>
     class Interval {};

     template<class I>
     class Base 
        {
          public:
               typedef I Interval;
               typedef int Integer;
        };

     template<typename T>
     class Derived: public Base<Interval<T> > 
        {
       // Is "Interval" test::Interval or test::Base::Interval
          typedef Interval<T> Test1; // Fails for Microsoft Visual Studio 2013
          typedef Interval Test2;    // Fails for GCC 4.4.5
        };

   } // namespace
