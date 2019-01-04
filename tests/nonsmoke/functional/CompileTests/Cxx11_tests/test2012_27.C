// Explicitly defaulted and deleted special member functions

// In C++03, the compiler provides, for classes that do not provide them for themselves, a default constructor, 
// a copy constructor, a copy assignment operator (operator=), and a destructor. The programmer can override 
// these defaults by defining custom versions. C++ also defines several global operators (such as operator= and 
// operator new) that work on all classes, which the programmer can override.

// However, there is very little control over the creation of these defaults. Making a class inherently non-copyable, 
// for example, requires declaring a private copy constructor and copy assignment operator and not defining them. 
// Attempting to use these functions is a violation of the one definition rule. While a diagnostic message is not 
// required,[11] this typically results in a linker error.[citation needed]

// In the case of the default constructor, the compiler will not generate a default constructor if a class is 
// defined with any constructors. This is useful in many cases, but it is also useful to be able to have both 
// specialized constructors and the compiler-generated default.

// C++11 allows the explicit defaulting and deleting of these special member functions. For example, the following 
// type explicitly declares that it is using the default constructor:

typedef int OtherType;

struct SomeType {
    SomeType() = default; //The default constructor is explicitly stated.
    SomeType(OtherType value);
};

// Alternatively, certain features can be explicitly disabled. For example, the following type is non-copyable:

struct NonCopyable {
    NonCopyable & operator=(const NonCopyable&) = delete;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable() = default;
};

// The = delete specifier can be used to prohibit calling any function, which can be used to disallow calling a 
// member function with particular parameters. For example:

struct NoInt {
    void f(double i);
    void f(int) = delete;
};

// An attempt to call f() with an int will be rejected by the compiler, instead of performing a silent conversion 
// to double. This can be generalized to disallow calling the function with any type other than double as follows:

struct OnlyDouble {
    void f(double d);
    template<class T> void f(T) = delete;
};


