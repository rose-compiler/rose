#if 0

>> File-Name: /home/andreas/REPOSITORY-BUILD/gcc41/mozilla/nsprpub/config/out.c
Inside of propagateHiddenListData (node = 0x8071618 = SgFile)
Error: default reached in In SgPointerDerefExp::get_type() someType =
SgFunctionType
Inside of Sg_File_Info::display(location of error: debug)
    isTransformation                      = false
    isCompilerGenerated                   = false
    isOutputInCodeGeneration              = false
    isShared                              = false
    isFrontendSpecific                    = false
    isSourcePositionUnavailableInFrontend = false
    isCommentOrDirective                  = false
    isToken                               = false
    filename =
/home/andreas/REPOSITORY-BUILD/gcc41/mozilla/nsprpub/config/out.c
    line     = 2  column = 11
    file_id  = 0
    filename =
/home/andreas/REPOSITORY-BUILD/gcc41/mozilla/nsprpub/config/out.c
    line     = 2  column   = 11
Inside of Sg_File_Info::display(location of parent: debug)
    isTransformation                      = false
    isCompilerGenerated                   = false
    isOutputInCodeGeneration              = false
    isShared                              = false
    isFrontendSpecific                    = false
    isSourcePositionUnavailableInFrontend = false
    isCommentOrDirective                  = false
    isToken                               = false
    filename =
/home/andreas/REPOSITORY-BUILD/gcc41/mozilla/nsprpub/config/out.c
    line     = 2  column = 11
    file_id  = 0
    filename =
/home/andreas/REPOSITORY-BUILD/gcc41/mozilla/nsprpub/config/out.c
    line     = 2  column   = 11
lt-identityTranslator: Cxx_Grammar.C:84518: virtual SgType*
SgPointerDerefExp::get_type() const: Assertion false failed.
Aborted (core dumped) 
#endif


void rmdir (){ 
   int test;
   (void) (test ? rmdir : rmdir)();   
} ;
