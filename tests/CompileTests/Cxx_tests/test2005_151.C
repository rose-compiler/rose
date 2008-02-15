// Similar problem as in test2005_150.C (reported as bug to GNU)
// Unparses: 
//      template<> const int B::blah() const;
//      template<> const int B::blah() const;
// twice instead of once!

struct B
   {
     template<typename T> T blah();
     template<typename T> T const blah() const;
   };

template<> int const B::blah() const;
