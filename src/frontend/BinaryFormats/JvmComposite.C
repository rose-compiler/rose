/* SgJvmComposite is container for a Java class or jar file.  */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

using std::string;
using std::vector;
using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

SgJvmComposite::SgJvmComposite(vector<string> & argv, SgProject* project)
  : SgBinaryComposite(argv, project)
{
  set_sourceFileUsesJvmFileExtension(true);
  set_inputLanguage(SgFile::e_Jvm_language);
  set_outputLanguage(SgFile::e_Jvm_language);

  // Java JVM files are not compilable
  set_Jvm_only(true);
  set_skip_syntax_check(true);
  set_skipfinalCompileStep(true);
  set_disable_edg_backend(true);
  set_skip_translation_from_edg_ast_to_rose_ast(true);
}

int
SgJvmComposite::callFrontEnd()
{
  // Process command line options specific to ROSE. This leaves
  // all filenames and non-rose specific options in the argv list.
  vector<string> argv{get_originalCommandLineArgumentList()};
  processRoseCommandLineOptions(argv);

  // TODO: unparsing
  set_skip_unparse(true);

  return buildAST(argv, vector<string>{});
}

/* Builds entire AST for a SgJvmComposite node. */
int
SgJvmComposite::buildAST(vector<string> argv, vector<string> /*inputCommandLine*/)
{
  int result{1};

  // argv is expected to contain executable and file name
  ROSE_ASSERT(argv.size() == 2);
  if (CommandlineProcessing::isJvmFileNameSuffix(Rose::StringUtility::fileNameSuffix(argv[0]))) {
    mlog[WARN] << "In SgJvmComposite::buildAST(): expected argv[0] to be path "
               << "to command line executable: is " << argv[0] << std::endl;
    return result;
  }

  auto gf = new SgAsmGenericFile{};
  gf->parse(argv[1]); /* this loads file into memory, does no reading of file */

  auto header = new SgAsmJvmFileHeader(gf);
  if (header->parse()) {
    p_genericFileList->get_files().push_back(gf);
    result = 0;
  }
  return result;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
