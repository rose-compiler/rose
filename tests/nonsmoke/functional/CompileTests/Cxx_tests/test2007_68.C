#if 0
when compiling the attached code I get the following error:
>> File-Name: /home/andreas/REPOSITORY-BUILD/gcc41/mozilla/nsprpub/pr/src/misc/prerrortable.c
Inside of propagateHiddenListData (node = 0x8074938 = SgFile)
Error: unexpected type found for likelyFunctionType = 0x81a34a0 = SgTypedefType
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
/home/andreas/REPOSITORY-BUILD/gcc41/mozilla/nsprpub/pr/src/misc/prerrortable.c
    line     = 9  column = 2
    file_id  = 0
    filename =
/home/andreas/REPOSITORY-BUILD/gcc41/mozilla/nsprpub/pr/src/misc/prerrortable.c
    line     = 9  column   = 2
lt-identityTranslator: Cxx_Grammar.C:61153: virtual SgType*
SgFunctionCallExp::get_type() const: Assertion `functionType !=
__null' failed.
Aborted (core dumped) 
#endif

typedef char PRErrorCallbackLookupFn();

static PRErrorCallbackLookupFn *callback_lookup;

void
voidF()
{
	callback_lookup();
}
