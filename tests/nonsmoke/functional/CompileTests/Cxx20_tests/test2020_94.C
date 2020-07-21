template<class... Args>           void f(Args... args);               // #1
template<class T1, class... Args> void f(T1 a1, Args... args);        // #2
template<class T1, class T2>      void f(T1 a1, T2 a2);               // #3
f();                  // calls #1
f(1, 2, 3);           // calls #2
f(1, 2);              // calls #3; non-variadic template #3 is more
                      // specialized than the variadic templates #1 and #2

