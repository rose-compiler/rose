/*
The following compiled as  identityCompiler -c bug.C -o bug.o

extern int operator_takes_lvalue_operand(int op);

gives the following error message from ROSE:

In SgType::mangledNameSupport(): This case should never be reached (fname = operator_takes_lvalue_operand)
identityTranslator: Cxx_Grammar.C:124397: SgName
SgType::mangledNameSupport(SgName&, SgUnparse_Info&): Assertion `false'
failed.
/home/saebjornsen1/links/g++3.3.3: line 4: 10755 Aborted                
/home/saebjornsen1/ROSE-October/gcc3.3.3/exampleTranslators/documentedExamples/simpleTranslatorExamples/identityTranslator
$@

Andreas
*/

extern int operator_takes_lvalue_operand(int op);
