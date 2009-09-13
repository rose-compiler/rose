/*
Hi Dan,
I get the following error:
Error: unexpected type found for likelyFunctionType = 0x933eb4c =
SgTypedefType
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
/home/andreas/REPOSITORY-BUILD/gcc-34/mozilla/nsprpub/pr/src/misc/output.c
     line     = 22  column = 4
     file_id  = 0
     filename =
/home/andreas/REPOSITORY-BUILD/gcc-34/mozilla/nsprpub/pr/src/misc/output.c
     line     = 22  column   = 4
lt-identityTranslator: Cxx_Grammar.C:44338: virtual SgType*
SgFunctionCallExp::get_type() const: Assertion `functionType != __null'
failed.
/home/andreas/links/gcc34: line 3: 20690 Aborted                
/home/andreas/REPOSITORY-BUILD/gcc-34/ROSE/January-2b/tutorial/identityTranslator
-rose:C $@

when compiling

 */

typedef const char *
PRErrorCallbackLookupFn(int code, unsigned int language,
     const struct PRErrorTable *table,
     struct PRErrorCallbackPrivate *cb_private,
     struct PRErrorCallbackTablePrivate *table_private);

struct PRErrorTableList {
    const struct PRErrorTable *table;
    struct PRErrorCallbackTablePrivate *table_private;
};

static struct PRErrorCallbackPrivate *callback_private = 0;
static PRErrorCallbackLookupFn *callback_lookup = 0;

void
PR_ErrorToString(int code, unsigned int language)
{

    struct PRErrorTableList *et;
    char *cp;

   callback_lookup(code, language, et->table,
      callback_private, et->table_private);

}
