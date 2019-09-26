// DQ (2/19/2019): This does not appear to be valid C++11 (or C++14 or C++17)

void foobar()
   {
  // alias template
     template<class T>
     using ptr = T*; 
  // the name 'ptr<T>' is now an alias for pointer to T
     ptr<int> x;
   }

