/*
compiling the following code in ROSE

typedef const char * PRErrorCallbackLookupFn();

static PRErrorCallbackLookupFn *callback_lookup;

void PR_ErrorToString()
{
 callback_lookup();
}

I get the following error:
Error: unexpected type found for likelyFunctionType = 0x9c5a9f8 = SgTypedefType
Inside of Sg_File_Info::display(Location of call to
SgFunctionCallExp::get_type(): debug)
    isTransformation                      = false
    isCompilerGenerated                   = false
    isOutputInCodeGeneration              = false
    isShared                              = false
    isFrontendSpecific                    = false
    isSourcePositionUnavailableInFrontend = false
    isCommentOrDirective                  = false
    isToken                               = false
    filename =
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/mozilla/nsprpub/pr/src/misc/test.c
    line     = 7  column = 3
    file_id  = 0
    filename =
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/mozilla/nsprpub/pr/src/misc/test.c
    line     = 7  column   = 3
lt-identityTranslator: Cxx_Grammar.C:61264: virtual SgType*
SgFunctionCallExp::get_type() const: Assertion functionType != __null failed.
/home/andreas/links/gcc-411: line 2: 17330 Aborted
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/June-29a-2007-Unsafe/tutorial/identityTranslator
-rose:C99 "$@" 
*/


typedef const char * PRErrorCallbackLookupFn();

static PRErrorCallbackLookupFn *callback_lookup;

void PR_ErrorToString()
   {
  // Both of these forms of a function call from a function pointer are the same internally in the AST.
     callback_lookup();
     (*callback_lookup)();
   }
