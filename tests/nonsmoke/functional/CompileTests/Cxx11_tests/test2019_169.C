struct X
   {
      typedef enum x_enum { one,two,three } numbers_type;
      numbers_type numvar;
   };

#if 0
X::numbers_type global_numvar1;
#endif

#if 1
typedef X::numbers_type global_numbers_type;
#endif

#if 0
global_numbers_type global_numvar2;
#endif
