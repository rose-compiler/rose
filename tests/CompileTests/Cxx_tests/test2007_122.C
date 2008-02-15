/*
Hi Dan

Last week, I was working on my checkers, no side effects inside sizeof operators, and
found an interesting test case which the ROSE frontend can't parse.

int bar();

void foo()
{
 int a = 14;
 int b = sizeof(a++);
 int c;

 a = sizeof(b--);
 //a = sizeof(--b);

 //c = sizeof((b=a+1));
 //int a = sizeof((b=bar()));
}

The last three commented lines can't be parsed in the frontend.

Here is the error message;
=======================================================
noSideEffectInSizeofTest: ../../../../ROSE-src/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:19885: SgExpression* sage_gen_expr(an_expr_node*, a_boolean, DataRequiredForComputationOfSourcePostionInformation*): Assertion `operand_1 != __null' failed.
Aborted
=======================================================

I've asked Gergo and Jeremiah and they suggested me to send you this issue.

Thank you

-- 
Han Suk Kim

Summer Student
@Lawrence Livermore National Laboratory

*/


int bar();

void foo()
{
 int a = 14;
 int b = sizeof(a++);
 int c;

 a = sizeof(b--);
 a = sizeof(--b);

 //c = sizeof((b=a+1));
 //int a = sizeof((b=bar()));
}
