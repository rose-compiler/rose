/*
Email from Andreas (high priority bug):

compiling the following code with the attached translator
namespace std
{
 template<typename _Tp>
   class vector
   {
   };

 template<typename _Tp>
   inline bool
   operator==(const vector<_Tp>& __x, const vector<_Tp>& __y)
   { return true; }
}


class CFGPath {
 std::vector<CFGPath> edges;
 public:

 bool foo() {return edges == edges;}
};

gives me the following error

Error: declarationAssociatedWithSymbol->get_symbol_from_symbol_table() == NULL
    declarationAssociatedWithSymbol = 0xb79c5008 =
SgTemplateInstantiationFunctionDecl = operator==
Inside of Sg_File_Info::display(declarationStatement)
    isTransformation                      = false
    isCompilerGenerated                   = false
    isOutputInCodeGeneration              = false
    isShared                              = false
    isFrontendSpecific                    = false
    isSourcePositionUnavailableInFrontend = false
    isCommentOrDirective                  = false
    isToken                               = false
    filename =
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/boost-June-14a-2007-Unsafe/tutorial/sageHeaderOnly.C
    line     = 19  column = 28
Inside of Sg_File_Info::display(declarationAssociatedWithSymbol)
    isTransformation                      = false
    isCompilerGenerated                   = false
    isOutputInCodeGeneration              = false
    isShared                              = false
    isFrontendSpecific                    = false
    isSourcePositionUnavailableInFrontend = false
    isCommentOrDirective                  = false
    isToken                               = false
    filename =
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/boost-June-14a-2007-Unsafe/tutorial/sageHeaderOnly.C
    line     = 10  column = 5
lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-14a-Unsafe/NEW_ROSE/src/backend/unparser/name_qualification_support.C:362:
SgName Unparser::generateNameQualifier(SgDeclarationStatement*, const
SgUnparse_Info&, bool): Assertion
declarationAssociatedWithSymbol->get_symbol_from_symbol_table() != __null failed.
Aborted 

*/





