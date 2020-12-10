// DQ (7/21/2020): Moved function calls into a function.
template <typename T>
void foobar1()
   {
     int f = 42;
     T* a = new T;      // calls operator new(sizeof(T))
                 // (C++17) or operator new(sizeof(T), std::align_val_t(alignof(T))))
     T* b = new T[5];   // calls operator new[](sizeof(T)*5 + overhead)
                 // (C++17) or operator new(sizeof(T)*5+overhead, std::align_val_t(alignof(T))))
     T* c = new(2,f) T; // calls operator new(sizeof(T), 2, f)
                 // (C++17) or operator new(sizeof(T), std::align_val_t(alignof(T)), 2, f)
   }


