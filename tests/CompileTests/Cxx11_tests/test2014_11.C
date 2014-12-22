class foo
{
//members...
public:
    foo(){}
    foo(const foo& copy)=default; // copy constructor

#if 0
 // These are not allowed (in gnu version 4.8.1 or in EDG 4.9).
 // Not clear if these made there way into the C++11 standard.
    foo(const foo&& move)=default; // move constructor
#else
    foo(const foo&& move); // move constructor
#endif

   ~foo(){}// destructor

#if 0
 // These are not allowed (in gnu version 4.8.1 or in EDG 4.9).
 // Not clear if these made there way into the C++11 standard.
   ~foo(const foo& copy); // copy destructor
   ~foo(const foo&& move); // move destructor
#endif
};

