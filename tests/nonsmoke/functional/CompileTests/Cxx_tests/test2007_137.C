/*
Bug name:       interaction-between-extern-decl-and-default-args

Reported by:    Brian White

Date:           Sept 29, 2007

Date sent:      Sept 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:      SPEC CPU 2006   471.omnetpp/src/libs/sim/util.cc
                                471.omnetpp/src/libs/sim/distrib.cc

Brief description:      A function with default arguments is declared
                        in a header file.  ROSE emits an extern
                        declaration in the source file that #include's
                        the header, which also includes the default
                        args.  This causes an error with the backend
                        compiler.


Files
-----
bug.h           Declares a function opp_concat with default args.

bug.cc          #include's bug.h and defines opp_concat.

                Compilation outputs the following errors:

rose_bug.cc:2: error: default argument given for parameter 3 of `char* 
   opp_concat(const char*, const char*, const char*, const char*)'
bug.h:1: error: after previous specification in `char* opp_concat(const char*, 
   const char*, const char*, const char*)'
rose_bug.cc:2: error: default argument given for parameter 4 of `char* 
   opp_concat(const char*, const char*, const char*, const char*)'
bug.h:1: error: after previous specification in `char* opp_concat(const char*, 
   const char*, const char*, const char*)'

rose_bug.cc     The output from the backend for bug.cc.  In it, we see
                that ROSE emits an extern declaration in the file rose_bug.cc,
                which redundatly includes the default args that are 
                already present in bug.h:

extern char *opp_concat(const char *s1,const char *s2,const char *s3=(0),const c
har *s4=(0));
*/

// DQ (10/1/2007): I think that we can avoid expressing this bug using a header file.
// Upon reflection, this test code need to be expressed using a header file so that 
// the function prototype will be seen twice.

#include "test2007_137.h"
// char *opp_concat(const char *s1, const char *s2, const char *s3=0, const char *s4=0);

char *opp_concat(const char *s1,
                 const char *s2,
                 const char *s3,
                 const char *s4)
{
}
