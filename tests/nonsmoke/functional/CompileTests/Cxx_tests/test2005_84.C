/*
If I write a statement such as "a = 5, b = 6;", ROSE seems to translate
this into an SgExprListExp rather than an SgCommaOpExp.  Is this correct?
It would seem that, even at the top level of a statement, that the comma
operator is still being used.  A statement such as "c = (a = 5, b = 6);"
produces SgCommaOpExp as expected.  The problem I see with this is that
the comma operator enforces a sequence among its operands, while an
expression list can evaluate them in any order.

							Jeremiah Willcock
*/

void foo()
   {
     int a,b,c;

  // Should this be a SgCommaOpExp or a SgExprListExp 
  // (it is a SgExprListExp, but should be a SgCommaOpExp)
     a = 5, b = 6;

  // Here it is a SgCommaOpExp as expected
     c = (a = 5, b = 6);
   }
