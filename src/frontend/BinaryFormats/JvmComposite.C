/* SgJvmComposite is container for a Java class or jar file.  */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
#include <util/Sawyer/FileSystem.h>

using std::string;
using std::vector;
using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

static
vector<string> getFilesInDir(string &path, string &extension) {
  vector<string> files;
  boost::filesystem::path dir(path);
  if (boost::filesystem::exists(path) && boost::filesystem::is_directory(path)) {
    boost::filesystem::recursive_directory_iterator it(path);
    boost::filesystem::recursive_directory_iterator endit;

    while (it != endit) {
      if (boost::filesystem::is_regular_file(*it) && (extension=="")?true:it->path().extension() == extension) {
        files.push_back(it->path().string());
      }
      ++it;
    }
  }
  return files;
}

// Extract class files from a jar file
static vector<string> extractClassFiles(string &jar)
{
  vector<string> classes{};

  if (!CommandlineProcessing::isJavaJarFile(jar)) {
    mlog[WARN] << "In SgJvmComposite::extractClassFiles(): expected file to be path "
               << "to a jar file: is " << jar << std::endl;
    return classes;
  }
  else {
    mlog[WARN] << "In SgJvmComposite::buildAST(): jar files not fully support yet, "
               << "name is " << jar << std::endl;
  }

  // Make system call to extract the class files
  Sawyer::FileSystem::TemporaryDirectory tmpDir{};
  // TODO: work around this (by passing temp directory in?
  tmpDir.keep(true);
  std::cout << "---> extractClassFiles: tmp directory is " << tmpDir.name() << std::endl;

  string commandString{};
  commandString += "cd " + tmpDir.name().string() + "; ";
  commandString += "jar xvf " + jar + "; ";
  commandString += "cd -";
  std::cout << "---> extractClassFiles: running system command: " << commandString << std::endl;

  if (system(commandString.c_str()) != 0) {
    mlog[WARN] << "In SgJvmComposite::buildAST(): failed system command: " << commandString << std::endl;
  }

  // TODO: add paths to extracted class files

  return classes;
}

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
  vector<string> classes{};

  // argv is expected to contain executable and file name (for now, TODO: multiple files)
  ROSE_ASSERT(argv.size() == 2);
  string fileName{argv[1]};

  // Obtain/extract class file(s)
  if (CommandlineProcessing::isJavaJvmFile(fileName)) {
    classes.push_back(fileName);
  }
  else {
    mlog[WARN] << "In SgJvmComposite::buildAST(): expected fileName to be path "
               << "to class or jar file: is " << fileName << std::endl;
    return result;
  }

  // Parse the class files
  for (auto className : classes) {
    auto gf = new SgAsmGenericFile{};
    gf->parse(className); /* this loads file into memory, does no reading of file */

    auto header = new SgAsmJvmFileHeader(gf);
    if (header->parse()) {
      p_genericFileList->get_files().push_back(gf);
      result = 0;
    }
  }

  return result;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
