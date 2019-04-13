// Building variations on the original test: test2019_239.C
// ROSE-1905: (C++03) template specialization declaration cannot be a friend

template <typename T> class hash;

struct error_code 
   {
     private:
          friend class hash<error_code>;
   };

// When this is a forward declaration the friend declaration disappears from the class.
// template<> struct hash<error_code> {};
template<> struct hash<error_code>;
