typedef unsigned long long size_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

// extern int parse_flag ( const char *text __attribute__ (( unused )), int *flag );
int parse_flag ( const char *text, int *flag );

struct option_descriptor {
 const char *longopt;
 char shortopt;
 uint8_t has_arg;
 uint16_t offset;
 int ( * parse ) ( const char *text, void *value );
};

struct echo_options {
 int no_newline;
};

enum getopt_argument_requirement {
 no_argument = 0,
 required_argument = 1,
 optional_argument = 2,
};

static struct option_descriptor echo_opts[] = 
   {
        { .longopt = "n", 
          .shortopt = 'n', 
          .has_arg = no_argument, 
          .offset = __builtin_offsetof(struct echo_options, no_newline), 
       // Original code:
       // .parse = ( ( int ( * ) ( const char *text, void *value ) ) ( ( ( ( typeof ( parse_flag ) * ) ((void *)0) ) == ( ( int ( * ) ( const char *text, typeof ( ( ( struct echo_options * ) ((void *)0) )->no_newline ) * ) ) ((void *)0) ) ) ? parse_flag : parse_flag ) ), 
       // .parse = ( ( int ( * ) ( const char *text, void *value ) ) ( ( ( ( typeof ( parse_flag ) * ) ((void *)0) ) == ( ( int ( * ) ( const char *text, typeof ( ( ( struct echo_options * ) ((void *)0) )->no_newline ) * ) ) ((void *)0) ) ) ? parse_flag : parse_flag ) ), 

       // Original code:
       // .parse = ( ( int ( * ) ( const char *text, void *value ) ) ( ( 0L == ( ( int ( * ) ( const char *text, typeof ( ( ( struct echo_options * ) ((void *)0) )->no_newline ) * ) ) ((void *)0) ) ) ? parse_flag : parse_flag ) ), 
          .parse = ( ( int ( * ) ( const char *text, void *value ) ) ( ( 0L == ( ( int ( * ) ( const char *text, typeof ( ( ( struct echo_options * ) ((void *)0) )->no_newline ) * ) ) ((void *)0) ) ) ? parse_flag : parse_flag ) ), 
        },
   };

void foobar()
   {
     typeof ( ( ( struct echo_options * ) ((void *)0) )->no_newline ) * abc;
     int ( *abc_function_ptr ) ( const char *text, typeof(((struct echo_options *)((void *)0)) -> no_newline) ) ;

     if ( 0L == ( ( int ( * ) ( const char *text, typeof ( ( ( struct echo_options * ) ((void *)0) )->no_newline ) * ) ) ((void *)0) ) )
        {
        }
   }
