/*
Bug name:       unparse-problem-with-operator-

Reported by:    Brian White

Date:           Sept 28, 2007

Date sent:      Sep 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:      SPEC CPU 2006   444.namd/src/dvector.cc

Brief description:      ROSE backend triggers an assertion failure
                        while attempting to unparse an invocation
                        of (friend) operator-.

Files
-----
bug.cc          Invokes -w in main, where w is a DVector and
                operator- has been declared a friend of DVector.

                This is the error returned when we try to compile
                the file rose_bug.cc:

rosec: /fusion/research4/bwhite/ROSE-0.8.10e/ROSE/src/backend/unparser/unparse_expr.C:34: virtual void Unparser::unparseExpression(SgExpression*, SgUnparse_Info&): Assertion `expr != __null' failed.

rose_bug.cc     The output from the backend for bug.cc.  ROSE unparses
                the AST up to the point of the -w, at which point it aborts.
*/


class DVector 
{
public:
   friend DVector operator-(const DVector& vec);

};

DVector operator-(const DVector& vec)
{
   DVector res;
   return res;
}

int main()
{
   DVector w;
   -w;
   return 0;
}
