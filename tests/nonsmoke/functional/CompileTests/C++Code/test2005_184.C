// This is a test of the extraction of comments and preprocessor control structure

// test a very \
very long string \
spanning several lines.

// another test of a very \
very long string "\\\\\\\\\\\\\\\\\
spanning several lines."

// another test of a string "\\"

const char* s = "\\";
#ifdef ANY_MACRO
// char s = "\1";
   int x = 42;
#else
   int y = 42;
#endif
   int z = 42;

