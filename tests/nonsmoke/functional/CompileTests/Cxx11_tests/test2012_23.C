// New string literals

// C++03 offers two kinds of string literals. The first kind, contained within double quotes, produces 
// a null-terminated array of type const char. The second kind, defined as L"", produces a null-terminated 
// array of type const wchar_t, where wchar_t is a wide-character. Neither literal type offers support for 
// string literals with UTF-8, UTF-16, or any other kind of Unicode encodings.

// For the purpose of enhancing support for Unicode in C++ compilers, the definition of the type char has 
// been modified to be both at least the size necessary to store an eight-bit coding of UTF-8 and large 
// enough to contain any member of the compiler's basic execution character set. It was previously defined 
// as only the latter.

// There are three Unicode encodings that C++11 will support: UTF-8, UTF-16, and UTF-32. In addition to the 
// previously noted changes to the definition of char, C++11 adds two new character types: char16_t and 
// char32_t. These are designed to store UTF-16 and UTF-32 respectively.

// The following shows how to create string literals for each of these encodings:

void foobar()
   {

     u8"I'm a UTF-8 string.";
     u"This is a UTF-16 string.";
     U"This is a UTF-32 string.";

// The type of the first string is the usual const char[]. The type of the second string is const char16_t[]. 
// The type of the third string is const char32_t[].

// When building Unicode string literals, it is often useful to insert Unicode codepoints directly into the 
// string. To do this, C++11 allows the following syntax:

     u8"This is a Unicode Character: \u2018.";
     u"This is a bigger Unicode Character: \u2018.";
     U"This is a Unicode Character: \u2018.";

// The number after the \u is a hexadecimal number; it does not need the usual 0x prefix. The identifier \u 
// represents a 16-bit Unicode codepoint; to enter a 32-bit codepoint, use \U and a 32-bit hexadecimal number. 
// Only valid Unicode codepoints can be entered. For example, codepoints on the range U+D800\u2014U+DFFF are 
// forbidden, as they are reserved for surrogate pairs in UTF-16 encodings.

// It is also sometimes useful to avoid escaping strings manually, particularly for using literals of XML 
// files, scripting languages, or regular expressions. C++11 provides a raw string literal:

     R"(The String Data \ Stuff " )";
     R"delimiter(The String Data \ Stuff " )delimiter";

// In the first case, everything between the "( and the )" is part of the string. The " and \ characters 
// do not need to be escaped. In the second case, the "delimiter( starts the string, and it ends only 
// when )delimiter" is reached. The string delimiter can be any string up to 16 characters in length, 
// including the empty string. This string cannot contain spaces, control characters, '(', ')', or 
// the '\' character. The use of this delimiter string allows the user to have ")" characters within 
// raw string literals. For example, R"delimiter((a-z))delimiter" is equivalent to "(a-z)".[4]

// Raw string literals can be combined with the wide literal or any of the Unicode literal prefixes:

     u8R"XXX(I'm a "raw UTF-8" string.)XXX";
     uR"*(This is a "raw UTF-16" string.)*";
     UR"(This is a "raw UTF-32" string.)";

   }
