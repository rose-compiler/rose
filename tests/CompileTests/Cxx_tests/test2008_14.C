struct __gconv_step_data
{
 struct __gconv_trans_data *__trans;
};



typedef struct __gconv_info
{
 int __nsteps;
 __extension__ struct __gconv_step_data __data [];
} *__gconv_t;

typedef union
{
 struct __gconv_info __cd;
 struct
 {
   struct __gconv_info __cd;
   struct __gconv_step_data __data;
 } __combined;
} _G_iconv_t; 

