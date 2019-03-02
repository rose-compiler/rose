// DQ (2/19/2019): This does not appear to be valid C++11 (or C++14 or C++17)

void foobar()
   {
  // type alias can introduce a member typedef name
     template<typename T>
     struct Container { using value_type = T; };
  // which can be used in generic programming
     template<typename Container>
     void g(const Container& c) { typename Container::value_type n; }
   }

