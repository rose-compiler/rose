#if 0

typedef union {
         struct sockaddr_x25 *__restrict __sockaddr_x25__;
       } __SOCKADDR_ARG __attribute__ ((__transparent_union__));

extern int accept ( __SOCKADDR_ARG __addr);

void pt_accept_cont()
{
    union { void* buffer; } arg2;
    accept(  arg2.buffer);
}

when compiling the attached code in C99 and C mode I get the following error:
lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-14a-2007/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:16991:
SgExprListExp* sage_gen_argument_list(an_expr_node*, a_type*, int,
DataRequiredForComputationOfSourcePostionInformation&): Assertion sourcePositionData.ok() == true failed.
Aborted (core dumped) 

#endif

#if __cplusplus
/* This test code is not defined to work for C++, it is not an error.
   This is a C or C99 code only!
 */
#warning "This is a C++ code!"
#else

#warning "This is not a C++ code!"

/* This is what it takes to make this code compile! */
/* # define __SOCKADDR_ARG		struct sockaddr *__restrict */

typedef union
           {
             struct sockaddr_x25 *__restrict __sockaddr_x25__;
           }
        SOCKADDR_ARG __attribute__ ((__transparent_union__));

extern int accept ( SOCKADDR_ARG __addr);

void pt_accept_cont()
   {
     union { void* buffer; } arg2;
     accept( arg2.buffer );
   }

#endif
