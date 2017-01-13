/*
The following program (extracted from mpi2c++):

struct A {operator int&();};
void f() {
  A x;
  (int)x;
}

produces the following assertion failure:

identityTranslator: /home/willcock2/ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:18347: 
SgExpression* sage_gen_expr(an_expr_node*, a_boolean, DataRequiredForComputationOfSourcePostionInformation*): Assertion sourcePositionData.ok() == true failed.
Abort

-- Jeremiah Willcock 
*/

struct A
    {
   // This is a conversion operator to a integer reference "int&"
   // Note: it can not coexist with a conversion operator to an integer.
      operator int &();

   // This is a conversion operator to a integer reference "int*"
   // operator int*();

   // This is the address operator
   // int operator &();

   // This the integer cast operator (can not coexist with "operator int &();")
   // operator int(); 
    };

void f()
   {
  // If this is a pointer then all the conversions are valid
     A x;

  // call the "operator int &();" This works!
  // (int&)x;

  // call the "operator int*();" This works only if the "operator int*();" is defined for class A!
  // (int*)x;

  // call the "operator int();" This fails in ROSE!
     (int)x;
   }
