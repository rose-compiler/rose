
typedef struct my_type {
   int min;
   int max;
} my_type_t;

#define MY_MACRO(a) ((a).max-(a).min+1);

void foobar()
   {
     int i;
     my_type_t *my_var;
     int lenmap;

  // This macro will represent dark tokens between statements (should be handled as such).
     lenmap = MY_MACRO((*my_var));

     { i = 0; }
   }

