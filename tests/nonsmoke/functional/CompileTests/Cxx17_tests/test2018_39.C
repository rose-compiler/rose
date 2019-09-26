// Removing Deprecated Exception Specifications from C++17

extern void f();  // potentially-throwing
     void g() noexcept {
       f();      // valid, even if f throws
       throw 42; // valid, effectively a call to std::terminate
     }

