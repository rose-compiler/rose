#if 0
ROSE gives the following bug message:

g++3.3.3 -c output.cpp  -o test.o
identityTranslator:
../../../../NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:16268:
SgExpression* sage_gen_expr(an_expr_node*, int): Assertion
`result->get_file_info()->ok()' failed.
/home/saebjornsen1/links/g++3.3.3: line 4: 23585 Aborted                
/home/saebjornsen1/ROSE-October/gcc3.3.3/exampleTranslators/documentedExamples/simpleTranslatorExamples/identityTranslator
$@

Andreas
#endif


class Complex
{
public:
  Complex (const Complex& y);
};
class MatSimplest_Complex
{
protected:
  Complex** A;
  int nrows;
  int ncolumns;
};

typedef Complex (*Func_Complex)(Complex value);

class Mat_Complex : public MatSimplest_Complex
{
public:
void apply (Func_Complex f);
};
void Mat_Complex:: apply (Func_Complex f)
{
   for (int i = 1; i <= nrows; i++)
     for (int j = 1; j <= ncolumns; j++)
       A[i][j] = f(A[i][j]);

}

