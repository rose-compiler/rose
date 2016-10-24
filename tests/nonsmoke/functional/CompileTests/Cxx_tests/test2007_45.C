/*
The following code (extracted from mpi2c++):

void f() {
  typedef int (A)();
  A *x;
  x();
}

causes the following error for me (skipping the uninitialized variable warning):

Error: unexpected type found for likelyFunctionType = 0x8d8ce04 = SgTypedefType
Inside of Sg_File_Info::display(Location of call to SgFunctionCallExp::get_type(): debug)
     isTransformation                      = false
     isCompilerGenerated                   = false
     isOutputInCodeGeneration              = false
     isShared                              = false
     isFrontendSpecific                    = false
     isSourcePositionUnavailableInFrontend = false
     isCommentOrDirective                  = false
     isToken                               = false
     filename = /tmp/mpi-h-preproc-4.c
     line     = 4  column = 3
     file_id  = 0
     filename = /tmp/mpi-h-preproc-4.c
     line     = 4  column   = 3
identityTranslator: Cxx_Grammar.C:44603: virtual SgType* SgFunctionCallExp::get_type() const: Assertion `functionType != __null' failed.
Abort

Does this happen in your version?

-- Jeremiah Willcock 
*/

void f()
   {
     typedef int (A)();
     A *x;
     x();
   }
