/*
Dan,
        Thanks so much for fixing the bugs I have submitted. I
think I have another one for you.  It looks like the ! operator
in C is being converted into both a SgNotOp and SgNotEqualOp
in some cases.  Below is a code snippet that should produce
this error when run through 'dotGenerator -rose:C'.  I have
also attached the .dot file that I produced.  You should see
that both a SgNotOp and SgNotEqualOp are placed in the
parse tree between the SgAssignOp and the SgFunctionCallExp.

        I found this in the context of code that uses strcmp()
as follows:  result = !strcmp(s1,s2);

        I really can't tell if this is an EDG bug or a ROSE
bug.

thanks
chadd

DQ: Verified problem
   For 
         x = !foo(1);
   What is generated in the AST is equivalent to:  
         x = !(foo(1) != 0);
   And what is unparsed is:
         x = !(foo(1) != 0);
   Which is incorrect!

*/

#if 0
/*********START CODE**********/
int foo(int x){

        x = !foo(1);
}
/**********END CODE**********/

#else

const int y = 0;
const int x = !0;

/* This works properly in C++ but not with -rose:C_only option */
int foo(int x, int y)
   {
     x = !y;
   }

#endif
