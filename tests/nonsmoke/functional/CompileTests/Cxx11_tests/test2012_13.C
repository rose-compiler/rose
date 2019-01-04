// Alternative function syntax
// Standard C function declaration syntax was perfectly adequate for the feature set of the C language. 
// As C++ evolved from C, it kept the basic syntax and extended it where necessary. However, as C++ became 
// more complicated, it exposed a number of limitations, particularly with regard to template function 
// declarations. The following, for example, is not allowed in C++03:

#if 0
// This is not valid code.
template<class Lhs, class Rhs>
  Ret adding_func(const Lhs &lhs, const Rhs &rhs) {return lhs + rhs;} //Ret must be the type of lhs+rhs
#endif

// The type Ret is whatever the addition of types Lhs and Rhs will produce. Even with the aforementioned 
// C++11 functionality of decltype, this is not possible:

#if 0
// This is not valid code.
template<class Lhs, class Rhs>
  decltype(lhs+rhs) adding_func(const Lhs &lhs, const Rhs &rhs) {return lhs + rhs;} //Not legal C++11
#endif

// This is not legal C++ because lhs and rhs have not yet been defined; they will not be valid identifiers 
// until after the parser has parsed the rest of the function prototype.

// To work around this, C++11 introduced a new function declaration syntax, with a trailing-return-type:

template<class Lhs, class Rhs>
  auto adding_func(const Lhs &lhs, const Rhs &rhs) -> decltype(lhs+rhs) {return lhs + rhs;}

// This syntax can be used for more mundane function declarations and definitions:

struct SomeStruct  {
    auto func_name(int x, int y) -> int;
};
 
auto SomeStruct::func_name(int x, int y) -> int {
    return x + y;
}

// The use of the keyword \u201cauto\u201d in this case means something different from its use in automatic type deduction.
