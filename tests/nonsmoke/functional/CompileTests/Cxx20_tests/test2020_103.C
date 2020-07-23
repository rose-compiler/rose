// DQ (7/21/2020): Moved function calls into a function.
template <typename T>
void foobar1()
   {
     char* ptr = new char[sizeof(T)]; // allocate memory
     T* tptr = new(ptr) T;            // construct in allocated storage ("place")
     tptr->~T();                      // destruct
     delete[] ptr;                    // deallocate memory
   }
