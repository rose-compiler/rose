/*
when compiling the following code:

typedef union {struct sockaddr_at *__restrict __sockaddr_at__; struct
sockaddr_ax25 *__restrict __sockaddr_ax25__;
      } __SOCKADDR_ARG __attribute__ ((__transparent_union__));

extern int accept (__SOCKADDR_ARG __addr);

void pt_accept_cont()
{
   void* buffer;
   accept(
    buffer);
}

I get the following output:

lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-29a-Unsafe/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:17166:
SgExprListExp* sage_gen_argument_list(an_expr_node*, a_type*, int,
DataRequiredForComputationOfSourcePostionInformation&): Assertion theArg == __null failed. 
*/

typedef union {
/*              struct sockaddr_at *__restrict __sockaddr_at__; */
                struct sockaddr_ax25 *__restrict __sockaddr_ax25__;
              } __SOCKADDR_ARG __attribute__ ((__transparent_union__));

extern int accept (__SOCKADDR_ARG __addr);

void pt_accept_cont()
{
   void* buffer;
   accept(
    buffer);
}  /* pt_accept_cont */
