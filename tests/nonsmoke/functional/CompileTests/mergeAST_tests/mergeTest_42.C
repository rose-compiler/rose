struct wait
   {
     struct {} __wait_terminated;
     struct { int x; };
  };

#if 1
struct
   {
     struct {} __wait_terminated;
     struct { int x; };
  };
#endif


#if 1
struct wait_alt
   {
     enum { X };
     enum { Y };
  };
#endif

