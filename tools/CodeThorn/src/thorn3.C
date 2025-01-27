// An example ROSE plugin
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include <boost/lexical_cast.hpp>

// Mandatory include headers
#include "rose.h"
#include "Rose/CommandLine.h"
#include "Sawyer/CommandLine.h"
#include "sageGeneric.h"

// Programmatic codethorn headers
#include "CodeThornLib.h"
#include "CodeThornOptions.h"
#include "TimeMeasurement.h"

// tool  specific
#include "AstTerm.h"
#include "CppStdUtilities.h"
#include "LanguageRestrictor.h"
#include "LanguageRestrictorCollection.h"

namespace ct  = CodeThorn;
namespace si  = SageInterface;
namespace scl = Sawyer::CommandLine;
using namespace CodeThorn;

const std::string thorn3version = "0.9.0";
using namespace std;

// required for some options in codethorn library (until removed)
CodeThorn::CommandLineOptions CodeThorn::args;

namespace {

} // anonymous namespace

class Thorn3Parser
{
public:
  struct Parameters
  {
    std::string astTermFileNamePrefix="astterm";
    bool mlAstTerm=false;
    bool astTerm=false;
    bool typedAstTerm=false;
    bool printVariants=false;
    bool printVariantSet=false;
    bool checkCLanguage=false;
  };

  /// sets the Thorn3Parser settings using the command line arguments
  /// \returns a list of unparsed arguments
  std::vector<std::string>
  parseArgs(std::vector<std::string> clArgs)
  {
    scl::Parser p = Rose::CommandLine::createEmptyParserStage("", "");

    // things like --help, --version, --log, --threads, etc.
    p.with(Rose::CommandLine::genericSwitches());
    //~ p.doc("Synopsis", "@prop{programName} [@v{switches}] @v{file_names}..."); // customized synopsis

    // Create a group of switches specific to this tool
    scl::SwitchGroup thorn3Parser("thorn3 - specific switches");

    thorn3Parser.name("thorn3");  // the optional switch prefix

    thorn3Parser.insert(scl::Switch("file-prefix")
                        .argument("filename", scl::anyParser(params.astTermFileNamePrefix))
                        .doc("filename prefix for all variants of ast term files. Provide an absolute paths if thorn3 is invoked in multiple different directories. For each ast term option a postfix ending in .txt is appended."));
    thorn3Parser.insert(scl::Switch("ast-term")
                        //.argument("selection", scl::booleanParser(params.astTerm))
                        //.intrinsicValue("true",scl::booleanParser(params.astTerm))
                        .intrinsicValue(true,params.astTerm)
                        .doc("single-line ast term"));
    thorn3Parser.insert(scl::Switch("ast-term-typed")
                        .intrinsicValue(true,params.typedAstTerm)
                        .doc("single-line typed ast term"));
    thorn3Parser.insert(scl::Switch("ast-term-indented")
                        .intrinsicValue(true,(params.mlAstTerm))
                        .doc("indented ast term"));
    thorn3Parser.insert(scl::Switch("print-variants")
                        .intrinsicValue(true,(params.printVariants))
                        .doc("print all SgNode variants of program with variant-id."));
    thorn3Parser.insert(scl::Switch("print-variant-set")
                        .intrinsicValue(true,(params.printVariantSet))
                        .doc("print all SgNode variants as variantT enum set. Can be used with language restrictor."));
    thorn3Parser.insert(scl::Switch("check-c")
                        .intrinsicValue(true,(params.checkCLanguage))
                        .doc("check if input program contains only  C language constructs."));

    p.purpose("Generates AST Term files")
      .doc("synopsis",
           "@prop{programName} [@v{switches}] @v{specimen_name}")
      .doc("description",
           "This program generates AST files in term format. The same term format can be used in the AstMatcher as input for matching AST subtrees.");
    scl::ParserResult cmdline = p.with(thorn3Parser).parse(clArgs).apply();

    return cmdline.unparsedArgs();
  }

  Parameters getParameters() {
    return params;
  }
private:
  Parameters params;
};

namespace
{
  struct CStringVector : private std::vector<char*>
  {
      using base = std::vector<char*>;

      CStringVector()
      : base()
      {}

      explicit
      CStringVector(const std::vector<std::string>& clArgs)
      : base()
      {
        base::reserve(clArgs.size());

        for (const std::string& s : clArgs)
        {
          const size_t sz         = s.size();
          char*        cstr       = new char[sz+1];
          const char*  char_begin = s.c_str();

          std::copy(char_begin, char_begin+sz, cstr);
          cstr[sz] = '\0';
          base::push_back(cstr);
        }
      }

      ~CStringVector()
      {
        for (char* elem : *this)
          delete elem;
      }

      using base::size;

      char** firstCArg() { return &front(); }
  };
}

int main( int argc, char * argv[] )
{
  using Sawyer::Message::mfacilities;

  int errorCode = 1;

  try
  {
    ROSE_INITIALIZE;
    ct::CodeThornLib::configureRose();

    std::vector<std::string> cmdLineArgs{argv+0, argv+argc};
    Thorn3Parser thorn3Parser;
    CStringVector unparsedArgsCStyle(thorn3Parser.parseArgs(std::move(cmdLineArgs)));
    Thorn3Parser::Parameters params=thorn3Parser.getParameters();
    //if(!(params.astTerm||params.typedAstTerm||params.mlAstTerm||params.printVariants||params.printVariantSet)) {
    //  cout<<"No output selected. Use at least one of --thorn3:ast-term --thorn3:ast-term-typed --thorn3:ast-term-indented --print-variants --print-variant-set"<<endl;
    //  return 0;
    //}

    int thornArgc = unparsedArgsCStyle.size();
    char** thornArgv = unparsedArgsCStyle.firstCArg();

    //~ for (int i = 0; i < thornArgc; ++i)
      //~ std::cerr << thornArgv[i] << std::endl;

    CodeThornOptions         ctOpt;

    mfacilities.control(ctOpt.logLevel);
    //logTrace() << "Log level is " << ctOpt.logLevel << endl;
    ct::TimingCollector      tc;

    cout << "Parsing and creating AST started."<<endl;
    //~ tc.startTimer();
    SgProject* project = ct::CodeThornLib::runRoseFrontEnd(thornArgc,thornArgv,ctOpt,tc);
    ROSE_ASSERT(project);
    cout << "Parsing and creating AST finished."<<endl;

    string fileNamePrefix=params.astTermFileNamePrefix;
    //cout<<"DEBUG: "<<fileNamePrefix<<":"<<params.astTerm<<params.typedAstTerm<<params.mlAstTerm<<endl;

    if(params.printVariants) {
      LanguageRestrictor lang;
      LanguageRestrictor::VariantSet variantSet=lang.computeVariantSetOfProvidedAst(project);
      for(auto variant : variantSet) {
        cout<<"Variant "<<variant<<": "<<lang.variantToString(variant)<<endl;
      }
    }
    if(params.printVariantSet) {
      LanguageRestrictor lang;
      LanguageRestrictor::VariantSet variantSet=lang.computeVariantSetOfProvidedAst(project);
      cout<<"{"<<endl;
      for(auto variant : variantSet) {
        cout<<"V_"<<lang.variantToString(variant)<<","<<endl;
      }
      cout<<"}"<<endl;
    }

    if(params.astTerm) {
      string fileName=fileNamePrefix+".txt";
      string term=AstTerm::astTermWithNullValuesToString(project);
      if(CppStdUtilities::writeFile(fileName, term))
        cout<<"Generated ast term file "<<fileName<<endl;
      else
        cerr<<"Error: could not create file "<<fileName<<endl;
    }
    if(params.typedAstTerm) {
      string fileName=fileNamePrefix+"_typed.txt";
      string term=AstTerm::astTermWithNullValuesAndTypesToString(project);
      if(CppStdUtilities::writeFile(fileName, term))
        cout<<"Generated typed ast term file "<<fileName<<endl;
      else
        cerr<<"Error: could not create file "<<fileName<<endl;
    }
    if(params.mlAstTerm) {
      string fileName=fileNamePrefix+"_indent.txt";
      string term=AstTerm::astTermToMultiLineString(project);
      if(CppStdUtilities::writeFile(fileName, term))
        cout<<"Generated indented ast term file "<<fileName<<endl;
      else
        cerr<<"Error: could not create file "<<fileName<<endl;
    }

    if(params.checkCLanguage) {
      LanguageRestrictorC cLangRestrictor;
      bool programOK=cLangRestrictor.checkProgram(project);
      cout<<"C Program check: ";
      if(programOK)
        cout<<"PASS";
      else
        cout<<"FAIL";
      cout<<endl;
    }

    errorCode = 0;
  } catch(const std::exception& e) {
    msgError() << "Error: " << e.what() << endl;
  } catch(char const* str) {
    msgError() << "Error: " << str << endl;
  } catch(const std::string& str) {
    msgError() << "Error: " << str << endl;
  } catch(...) {
    msgError() << "Error: Unknown exception raised." << endl;
  }

  mfacilities.shutdown();
  return errorCode;
}

