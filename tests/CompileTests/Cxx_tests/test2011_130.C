/*
The input code is the following

-------------------------------
void foo(int a)
{
	&(a ? a : a);
}
-------------------------------

ROSE produces the following error when it tries to compile the input:


AAAAAAA new SgAddressOfOp #1 result = 0x106b1a400 
Assertion failed: (operand_1 != NULL), function sage_gen_expr, file
../../../../../../src/frontend/CxxFrontend/EDG/EDG_SAGE_Connection/sage_gen_be.C, line 12070.
*/



void foo(int a)
   {
     &(a ? a : a);
   }
