
// In this test the class iterator is shared in the typedef declaration.

template <typename T>
class X
   {
     public: 
       // This is an example of what is internally a nonreal class.
       // Until class X has been instantiated, there is no class iterator.
          class iterator {};

       // typedef iterator* iterator_ptr;
          typedef iterator iterator_ptr;
   };

