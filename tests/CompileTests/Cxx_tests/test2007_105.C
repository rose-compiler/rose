/*
When compiling the following code in ROSE

class jsdService
{
 public:
   jsdService() : mInitAtStartup(triUnknown)
   {
   }
 private:
   enum Tristate {
       triUnknown = 0U,
       triYes = 1U,
       triNo = 2U
   };

   Tristate    mInitAtStartup;

};


I get the following errror:

lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-29a-Unsafe/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_il_to_str.C:7108:
SgExpression* sage_form_constant(a_constant*, a_boolean,
an_il_to_str_output_control_block*,
DataRequiredForComputationOfSourcePostionInformation&): Assertion enumDecl != __null failed.
/home/andreas/links/g++-411: line 2: 15747 Aborted
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/June-29a-2007-Unsafe/tutorial/identityTranslator
*/

class jsdService
{
 public:
   jsdService() : mInitAtStartup(triUnknown)
   {
   }
 private:
   enum Tristate {
       triUnknown = 0U,
       triYes = 1U,
       triNo = 2U
   };

   Tristate    mInitAtStartup;

};
