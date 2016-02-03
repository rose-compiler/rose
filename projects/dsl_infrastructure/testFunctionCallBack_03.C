// Example from: http://wisol.ch/w/articles/2015-03-08-callbacks-in-cpp11/

// using cb1_t = std::function<void()>;
#include <functional>
#include <iostream>

// namespace {

using cb1_t = std::function<void()>;
using cb2_t = std::function<void(int)>;

// Wrapper function with std::function without arguments.
template<typename R>
void call(std::function<R(void)> f)
   {
     f();
   }

// Wrapper function with std::function with arguments.
template<typename R, typename ...A>
void call(std::function<R(A...)> f, A... args)
   {
     f(args...);
   }

// Wrapper function for generic callable object without arguments.
// Delegates to the std::function call.
template<typename R>
void call(R f(void))
   {
     call(std::function<R(void)>(f));
   }

// Wrapper function for generic callable object with arguments.
// Delegates to the std::function call.
template<typename R, typename ...A>
void call(R f(A...), A... args)
   {
     call(std::function<R(A...)>(f), args...);
   }

// Wrapper for a function pointer (e.g. a lambda without capture) without
// arguments.
using fp = void (*)(void);
void call(fp f)
   {
     call(std::function<void()>(f));
   }

void foo1()
   {
     std::cout << "foo1 is called\n";
   }

void foo2(int i)
   {
     std::cout << "foo2 is called with: " << i << "\n";
   }

// } // end anonymous namespace

int main()
   {
  // Call function 1.
     call(&foo1);

  // Alternative to call function 1.
     cb1_t f1 = std::bind(&foo1);
     call(f1);

  // Call function 2.
     call(&foo2, 5);

  // Alternative to call function 2.
     cb2_t f2 = std::bind(&foo2, std::placeholders::_1);
     call(f2, 5);

  // Here is an example with a lambda. It calls the function that takes a function pointer.
     call([] { std::cout << "lambda called\n"; });

     return 0;
   }


