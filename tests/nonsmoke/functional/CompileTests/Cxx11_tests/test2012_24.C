// User-defined literals

// C++03 provides a number of literals. The characters \u201c12.5\u201d are a literal that is resolved by 
// the compiler as a type double with the value of 12.5. However, the addition of the suffix \u201cf\u201d, 
// as in \u201c12.5f\u201d, creates a value of type float that contains the value 12.5. The suffix modifiers 
// for literals are fixed by the C++ specification, and C++ code cannot create new literal modifiers.

// C++11 also includes the ability for the user to define new kinds of literal modifiers that will construct 
// objects based on the string of characters that the literal modifies.

// Literals transformation is redefined into two distinct phases: raw and cooked. A raw literal is a sequence 
// of characters of some specific type, while the cooked literal is of a separate type. The C++ literal 1234, 
// as a raw literal, is this sequence of characters '1', '2', '3', '4'. As a cooked literal, it is the 
// integer 1234. The C++ literal 0xA in raw form is '0', 'x', 'A', while in cooked form it is the integer 10.

// Literals can be extended in both raw and cooked forms, with the exception of string literals, which can 
// be processed only in cooked form. This exception is due to the fact that strings have prefixes that affect 
// the specific meaning and type of the characters in question.

// All user-defined literals are suffixes; defining prefix literals is not possible.

// User-defined literals processing the raw form of the literal are defined as follows:

// OutputType operator "" _suffix(const char *literal_string);
 
OutputType some_variable = 1234_suffix;

// The second statement executes the code defined by the user-defined literal function. This function is 
// passed "1234" as a C-style string, so it has a null terminator.

// An alternative mechanism for processing integer and floating point raw literals is through a variadic template:

template<char...> OutputType operator "" _suffix();
 
OutputType some_variable = 1234_suffix;
OutputType another_variable = 2.17_suffix;

// This instantiates the literal processing function as operator "" _suffix<'1', '2', '3', '4'>(). In this form, 
// there is no terminating null character to the string. The main purpose to doing this is to use C++11's 
// constexpr keyword and the compiler to allow the literal to be transformed entirely at compile time, assuming 
// OutputType is a constexpr-constructable and copyable type, and the literal processing function is a constexpr 
// function.

// For numeric literals, the type of the cooked literal is either unsigned long long for integral literals or 
// long double for floating point literals. (Note: There is no need for signed integral types because a 
// sign-prefixed literal is parsed as expression containing the sign as unary prefix operator and the unsigned 
// number.) There is no alternative template form:

OutputType operator "" _suffix(unsigned long long);
OutputType operator "" _suffix(long double);
 
OutputType some_variable    = 1234_suffix;   // uses the first function
OutputType another_variable = 3.1416_suffix; // uses the second function

// For string literals, the following are used, in accordance with the previously mentioned new string prefixes:

OutputType operator "" _suffix(const char * string_values, size_t num_chars);
OutputType operator "" _suffix(const wchar_t * string_values, size_t num_chars);
OutputType operator "" _suffix(const char16_t * string_values, size_t num_chars);
OutputType operator "" _suffix(const char32_t * string_values, size_t num_chars);
 
OutputType some_variable = "1234"_suffix;      //Calls the const char * version
OutputType some_variable = u8"1234"_suffix;    //Calls the const char * version
OutputType some_variable = L"1234"_suffix;     //Calls the const wchar_t * version
OutputType some_variable = u"1234"_suffix;     //Calls the const char16_t * version
OutputType some_variable = U"1234"_suffix;     //Calls the const char32_t * version

// There is no alternative template form. Character literals are defined similarly.

