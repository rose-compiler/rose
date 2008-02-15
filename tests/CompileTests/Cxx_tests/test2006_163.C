
// enum x_enum { one,two,three } numbers_var;
// enum x_enum { one,two,three };

struct X
   {
      typedef enum x_enum { one,two,three } numbers_type;
      numbers_type numvar;
   };

X::numbers_type global_numvar1;

typedef X::numbers_type global_numbers_type;

global_numbers_type global_numvar2;

