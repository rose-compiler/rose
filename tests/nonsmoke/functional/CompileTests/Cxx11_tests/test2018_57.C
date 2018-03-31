template<class E>
class initializer_list
   {
  // implementation (a pair of pointers or a pointer + length)
     public:
          constexpr initializer_list(const E*, const E*); // [first,last)
          constexpr initializer_list(const E*, int); // [first, first+length)
          constexpr int size() const; // no. of elements
       // constexpr const T* begin() const; // first element
       // constexpr const T* end() const; // one more than the last element
          constexpr const E* begin() const; // first element
          constexpr const E* end() const; // one more than the last element
   };

