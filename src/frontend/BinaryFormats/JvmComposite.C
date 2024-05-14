/* SgJvmComposite is container for a Java class or jar file.  */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
#include <Sawyer/FileSystem.h>

using std::string;
using std::vector;
using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.

// Data structures to facilitate jar file processing and class lookup
// WARNING: HACK_ATTACH: Data structures need to go SOMEWHERE!
// Orthogonal to implementation, so they go here, blame me!
// Also: see ROSE/BinaryAnalysis/Partitioner2/ModulesJvm.h

std::map<string, SgAsmGenericFile*> jarMap; // probably a class member because ...
std::map<string, rose_addr_t> classMap; // maps to local class offset
bool compressed{false};
bool isJarFile{false};

#if 0
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
#endif

#if 0
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
#endif

SgJvmComposite::SgJvmComposite(vector<string> & argv, SgProject* project)
  : SgBinaryComposite(argv, project)
{
  set_sourceFileUsesJvmFileExtension(true);
  set_inputLanguage(SgFile::e_Jvm_language);
  set_outputLanguage(SgFile::e_Jvm_language);

  // Do JVM stuff only.  This perhaps should be left as a command line option.
  set_Jvm_only(true);

  // Java JVM files are not compilable
  set_skip_syntax_check(true);
  set_skipfinalCompileStep(true);
  set_skip_translation_from_edg_ast_to_rose_ast(true);

  // Don't do C++ stuff
  set_requires_C_preprocessor(false);
  set_disable_edg_backend(true);
  set_skip_commentsAndDirectives(true);
}

int
SgJvmComposite::callFrontEnd()
{
  // Process command line options specific to ROSE. This leaves THE filename
  // (files processed one at a time) and non-rose specific options in the argv list.
  vector<string> argv{get_originalCommandLineArgumentList()};
  processRoseCommandLineOptions(argv);

  return buildAST(argv, vector<string>{});
}

/* Builds entire AST for a SgJvmComposite node. */
int
SgJvmComposite::buildAST(vector<string> argv, vector<string> /*inputCommandLine*/)
{
  // Note, files are processed one at a time from the command line (argv[0] is the executable)
  // and argv[1] is the file. Note, there may be (non rose) options passed in argv?
  // For now assert argc == 2 and process argv[1]. For now, just save jar files in the SgAsmGenericFile list.
  // Since jar files are processed further here, jars must be a member (or other data structure, map, created later).

  ASSERT_require(argv.size() == 2);
  string path{argv[1]};
  boost::filesystem::path fsPath{path};
  string filename{fsPath.filename().string()};

  if (!CommandlineProcessing::isJavaJarFile(filename) && !CommandlineProcessing::isJavaClassFile(filename)) {
    // argv contains paths and non-rose command-line options
    mlog[WARN] << "In SgJvmComposite::buildAST(): expected path to be "
               << "to a class or jar file: is " << path << std::endl;
    return 1; // error status
  }

  // If path to file does not exists, search for class file in classpaths
  if (!boost::filesystem::exists(fsPath)) {
    SgProject* project{isSgProject(get_parent()->get_parent())};
    if (project) {
      std::list<std::string> classpath{project->get_Java_classpath()};
      for (auto &cp : classpath) {
        std::cerr << "classpath path: " << cp << "\n";
      }
    }
  }

  auto gf = new SgAsmGenericFile{};
  gf->parse(path); /* this loads file into memory, does no reading of file */
  p_genericFileList->get_files().push_back(gf);

  // Look for a class file
  if (CommandlineProcessing::isJavaClassFile(filename)) {
    auto header = new SgAsmJvmFileHeader(gf);
    header->parse();
    compressed = false; // This is true
    isJarFile = false; // This is also true
    classMap[filename] = 0; // MAKE SURE OF THIS
  }

  // Look elsewhere for a jar file (EngineJvm)
#if 0
  else if (CommandlineProcessing::isJavaJarFile(filename)) {
    // Go for broke
    Rose::BinaryAnalysis::Partitioner2::ModulesJvm::Zipper zip{gf->content(), /*classpath*/nullptr};

    // Really broke!
    for (auto &entry: zip.offsetMap_) {
      if (boost::filesystem::path{entry.first}.extension() == ".class") {
        classMap[filename] = entry.second;
        std::cerr << "  found class " << entry.first << " at offset : " << entry.second << std::endl;
      } else {
        std::cerr << "  found file  " << entry.first << " at offset : " << entry.second << std::endl;
      }
    }
    // TODO: replace with parse()?
    isJarFile = true;
  }
#endif

  // Sanity checks to see if logic of callFrontEnd and buildAST is correct
  ASSERT_require(p_genericFileList->get_files().size() == 1);

  return 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
