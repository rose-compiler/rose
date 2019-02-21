struct X
   {
     typedef enum x_enum { two } numbers_type;
  // numbers_type numvar;
   };

typedef X::numbers_type global_numbers_type;

