
/*
Email from Andreas:

when compiling the attached file in ROSE in C++ mode I get the following error:
.pp -I/home/andreas/REPOSITORY-SRC/PROJECTS-TO-COMPILE-WITH-ROSE/mozilla/xpcom/string/src/
test-enum.c
lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-14a-2007/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_il_to_str.C:6984:
SgExpression* sage_form_constant(a_constant*, a_boolean,
an_il_to_str_output_control_block*,
DataRequiredForComputationOfSourcePostionInformation&): Assertion
`enumSymbol != __null' failed.
Aborted (core dumped) 

*/


class nsTSubstring_CharT 
  {
    public:
      void IsVoid() 
        {
          F_VOIDED;
        }

      enum
        {
        // F_VOIDED        = 1 << 1
           F_VOIDED
        };

  };


