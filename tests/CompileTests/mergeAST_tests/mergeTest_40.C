// Case of AST island hidden in a variable declaration
struct __locale_struct_alt
   {
  // Note that the redundant index value from the merged AST is not deleted.  
  // This might be best done by the SgArrayType delete operator!
     struct locale_data *__locales[42];

  // Because this type is used in an AST island we don't see it with our current traversal!
     const unsigned short int *__ctype_b;
     const int *__ctype_tolower;
     const int *__ctype_toupper;

  /* Note: LC_ALL is not a valid index into this array.  */
     const char *__names[7];
   } *__locale_t_alt;

