- if use -rose:C_only, ROSE reports a false alarm 
  ("array containing unknown-sized type") on 
  "/usr/include/_G_config.h" for the code:

#include <gconv.h>
typedef union
{
  struct __gconv_info __cd;
  struct
  {
    struct __gconv_info __cd;
    struct __gconv_step_data __data;
  } __combined;
} _G_iconv_t;


Best regards,

Lingxiao Jiang


