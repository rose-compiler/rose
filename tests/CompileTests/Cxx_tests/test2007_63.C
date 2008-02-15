#if 0
Hi Dan,
when compiling the following code with ROSE compiled under gcc 4.1.1
template<typename _CharT>
class basic_istream
  {
};

template<typename _CharT>
class basic_streambuf
{
 public:

  template<typename _CharT2>
   friend basic_istream<_CharT2>&
   operator>>(basic_istream<_CharT2>&, _CharT2*);
};

extern template class basic_streambuf<char>;

template<>
basic_istream<char>&
operator>>(basic_istream<char>& __in, char* __s);


I get the following error message:


Error: in how SgTemplateInstantiationFunctionDecl is marked as
compiler generated
Inside of Sg_File_Info::display(Error: in how
SgTemplateInstantiationFunctionDecl is marked as compiler generated
(SgTemplateInstantiationFunctionDecl): debug)
    isTransformation                      = false
    isCompilerGenerated                   = false
    isOutputInCodeGeneration              = false
    isShared                              = false
    isFrontendSpecific                    = false
    isSourcePositionUnavailableInFrontend = false
    isCommentOrDirective                  = false
    isToken                               = false
    filename =
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/June-13a-2007-Unsafe/projects/DocumentationGenerator/sageHeaderOnly2.C
    line     = 13  column = 5
    file_id  = 0
    filename =
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/June-13a-2007-Unsafe/projects/DocumentationGenerator/sageHeaderOnly2.C
    line     = 13  column   = 5
Inside of Sg_File_Info::display(Error: in how
SgTemplateInstantiationFunctionDecl is marked as compiler generated
(SgTemplateDeclaration): debug)
    isTransformation                      = false
    isCompilerGenerated                   = true (no position information)
    isOutputInCodeGeneration              = false
    isShared                              = false
    isFrontendSpecific                    = false
    isSourcePositionUnavailableInFrontend = false
    isCommentOrDirective                  = false
    isToken                               = false
    file_id  = 0
    filename =
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/June-13a-2007-Unsafe/projects/DocumentationGenerator/sageHeaderOnly2.C
    line     = 12  column   = 5
lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-13a-Unsafe/NEW_ROSE/src/midend/astDiagnostics/AstConsistencyTests.C:1206:
virtual void TestAstTemplateProperties::visit(SgNode*): Assertion s->get_templateDeclaration()->get_file_info()->isCompilerGenerated() == false failed.
Aborted 

#endif

template<typename _CharT>
class basic_istream
  {
  };

template<typename _CharT>
class basic_streambuf
   {
     public:
          template<typename _CharT2> friend basic_istream<_CharT2>& operator>>(basic_istream<_CharT2>&, _CharT2*);
   };

// Error: This template declaration will not be output in the generated code.
// It appears to be in the AST, but is marked as compiler generated.
extern template class basic_streambuf<char>;

template<> basic_istream<char>& operator>>(basic_istream<char>& __in, char* __s);

