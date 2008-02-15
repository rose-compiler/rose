/*
The following code compiled with ROSE with the -rose:C_only option: 

extern
bool operator_takes_lvalue_operand(unsigned char op);

gives the following error:

In SgType::mangledNameSupport(): This case should never be reached (fname
= operator_takes_lvalue_operand)
identityTranslator: Cxx_Grammar.C:60877: SgName
SgType::mangledNameSupport(SgName&, SgUnparse_Info&): Assertion `false'
failed.
*/

extern
bool operator_takes_lvalue_operand(unsigned char op);
