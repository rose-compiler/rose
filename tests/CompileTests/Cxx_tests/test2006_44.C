
#if 0
- if use -rose:C_only, ROSE reports a false alarm 
  ("array containing unknown-sized type") on 
  "/usr/include/_G_config.h" for the code:

Best regards,
Lingxiao Jiang
#endif

// Cong (10/20/2010): OS X does not have this non-standard header file gconv.h
#ifndef __APPLE__

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

#endif
