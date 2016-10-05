#if 0
INITIAL BUG REPORT:

This  error is an attempt at synthetization from the next bug report. Look
at next post for a biggger example.

The following code:
struct __gconv_step_data
{
  unsigned char *__outbufend;
};

typedef struct __gconv_info
{
struct __gconv_step_data __data [];
} *__gconv_t;

int main(){
};

gives the following error in ROSE:

test.C", line 12: error: incomplete type is not allowed"
  struct __gconv_step_data __data [];
                           ^

Errors in EDG Processing!
/home/saebjornsen1/links/gcc3.3.3: line 4: 11076 Aborted                
/export/0/tmp.saebjornsen1/BUILDS/ROSE-January-6a/gcc3.3.3/exampleTranslators/documentedExamples/simpleTranslatorExamples/identityTranslator
-rose:C_only $@


RELATED BUG REPORT:

Referred to in previous bug report.
A possible reason for the problem is that the code is compiled in C mode,
while gcc actually allows C99 specific features.

The following code:
//VERSION 1: KEEP #include LINE and comment out #define LINE
#include <sys/cdefs.h>
//VERSION 2: KEEP #include LINE and comment out #define LINE
//Support for the following line requires support for C99 flexible array
members
//# define __flexarr      []

struct __gconv_step_data{
  unsigned char *__outbufend;
};

typedef struct __gconv_info{
  struct __gconv_step *__steps;
  __extension__ struct __gconv_step_data __data __flexarr;
} *__gconv_t;

typedef union{
  struct
  {
    struct __gconv_info __cd;
    struct __gconv_step_data __data;
  } __combined;
} _G_iconv_t;

gives the following error:
"test2.C", line 19: error: type containing an unknown-size array is not
allowed
      struct __gconv_info __cd;

#endif

struct __gconv_step_data
   {
     unsigned char *__outbufend;
   };

typedef struct __gconv_info
   {
     struct __gconv_step_data __data [];
   } *__gconv_t;


