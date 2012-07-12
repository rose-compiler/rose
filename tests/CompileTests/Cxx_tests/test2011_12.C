/*
The following code causes an assertion in name_qualification_support.C:453. It compiles fine with GCC.

structCastBug.C
---------------------------
struct transit
{
};

void make (void *trans)
{
  struct transit *transit = (struct transit *) trans;
}
---------------------------

The error is as follows:

lt-identityTranslator: ../../../src/backend/unparser/languageIndependenceSupport/name_qualification_support.C:453: SgName
Unparser_Nameq::generateNameQualifier(SgDeclarationStatement*, const SgUnparse_Info&, bool): Assertion
`info.get_declstatement_ptr() != __null || nestedQualification == true' failed.


It seems that the issue is that the name of the variable is the same as the name of the struct. However, this occurs
a lot in the code base I'm working with and other compilers seem to handle it fine.

-George
*/


struct transit
   {
   };

void make (void *trans)
   {
     struct transit *transit = (struct transit *) trans;
   }
