// Similar problem as in test2005_150.C (reported as bug to GNU)

struct B
   {
     template<typename T> T blah();
     template<typename T> T const blah() const;
   };

template<> int const B::blah() const;
