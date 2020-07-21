new T;      // calls operator new(sizeof(T))
            // (C++17) or operator new(sizeof(T), std::align_val_t(alignof(T))))
new T[5];   // calls operator new[](sizeof(T)*5 + overhead)
            // (C++17) or operator new(sizeof(T)*5+overhead, std::align_val_t(alignof(T))))
new(2,f) T; // calls operator new(sizeof(T), 2, f)
            // (C++17) or operator new(sizeof(T), std::align_val_t(alignof(T)), 2, f)

