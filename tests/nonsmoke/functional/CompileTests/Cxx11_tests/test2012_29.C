// Static assertions

// C++03 provides two methods to test assertions: the macro assert and the preprocessor directive #error. 
// However, neither is appropriate for use in templates: the macro tests the assertion at execution-time, 
// while the preprocessor directive tests the assertion during preprocessing, which happens before 
// instantiation of templates. Neither is appropriate for testing properties that are dependent on 
// template parameters.

// The new utility introduces a new way to test assertions at compile-time, using the new keyword 
// static_assert. The declaration assumes the following form:

// static_assert (constant-expression, error-message);

#include<type_traits>

// Here are some examples of how static_assert can be used:

const int GREEKPI = 4;

// static_assert((GREEKPI > 3.14) && (GREEKPI < 3.15), "GREEKPI is inaccurate!");
static_assert((GREEKPI > 2) && (GREEKPI < 5), "GREEKPI is inaccurate!");

template<class T>
struct Check  {
    static_assert(sizeof(int) <= sizeof(T), "T is not big enough!");
};

template<class Integral>
Integral foo(Integral x, Integral y) {
    static_assert(std::is_integral<Integral>::value, "foo() parameter must be an integral type.");
}

// When the constant expression is false the compiler produces an error message. The first example 
// represents an alternative to the preprocessor directive #error, in contrast in the second example 
// the assertion is checked at every instantiation of the template class Check.

// Static assertions are useful outside of templates as well. For instance, a particular implementation 
// of an algorithm might depend on the size of a long long being larger than an int, something the 
// standard does not guarantee. Such an assumption is valid on most systems and compilers, but not all.



