/* 
Example from Andreas from compiling Mozilla Firefox

Another problem with "struct __gconv_info __cd;"

Since the names that are generated for these examples are quite long, we could do an 
analysis to see if we really even needed the generated name and that would make the 
generated code much better.  In general we only need the generated names for un-named 
structs, classes and unions when there are multiple references to them.

*/


struct __gconv_step_data
{
  struct __gconv_trans_data *__trans;
};

typedef struct __gconv_info
{
  struct __gconv_step *__steps;
  __extension__ struct __gconv_step_data __data [];
} *__gconv_t;

typedef union
{
  struct
  {
    struct __gconv_info __cd;
    struct __gconv_step_data __data;
  } __combined;
} _G_iconv_t;


