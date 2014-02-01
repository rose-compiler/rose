void
update_path ()
   {
     const char *key;

  // In the macro expansion, CONST_CAST(char *, key) will expand to:
  // char* nonconst_key = ((__extension__(union {const char * _q; char * _nq;})((key)))._nq);
  // char* nonconst_key = CONST_CAST (char *, key);

  // Working with the expanded version of the macro for simplicity.
     char* nonconst_key = ((__extension__(union {const char * _q; char * _nq;})((key)))._nq);

  // This works in the case of a function argument (but fails for initializer case above).
     int status;
     foobar1((__extension__ (((union { int __in; int __i; }) { .__in = (status) }).__i)));
   }
