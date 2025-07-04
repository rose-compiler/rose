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
#include "CodeThornCommandLineOptions.h"
#include "TimeMeasurement.h"
#include "FunctionIdMapping.h"

// Class hierarchy analysis
#include "ClassHierarchyAnalysis.h"
#include "ObjectLayoutAnalysis.h"
#include "ClassHierarchyWriter.h"
#include "Memoizer.h"

namespace ct  = CodeThorn;
namespace si  = SageInterface;
namespace scl = Sawyer::CommandLine;

using namespace std;
using namespace CodeThorn;

const std::string thorn2version = "0.9.6";


std::string encodedName(std::string origname, const char* prefix, size_t maxlen)
{
  static size_t uid = 0;

  std::stringstream buf;
  size_t            len = std::min(maxlen, origname.length());

  buf << (maxlen ? origname.substr(0, len) : std::string{prefix})
      << (++uid);

  return buf.str();
}

std::string encodedKey(ClassKeyType key, ClassNameFn* fn, const char* prefix, size_t maxlen)
{
  return encodedName((*fn)(key), prefix, maxlen);
}

std::string encodedVar(VariableKeyType key, VarNameFn* fn, const char* prefix, size_t maxlen)
{
  return encodedName((*fn)(key), prefix, maxlen);
}

///
template <class GeneratorFunction>
struct NameGenerator;

/// For classes the memoization is applied to class key type
/// \details
///    different classes get different names
template <>
struct NameGenerator<ClassNameFn>
{
    NameGenerator(ClassNameFn gen, const char* nameprefix, size_t numCharsOfOriginalName)
    : memo(&encodedKey), nameGen(gen), prefix(nameprefix), maxlen(numCharsOfOriginalName)
    {}

    std::string operator()(ClassKeyType key)
    {
      return memo(key, &nameGen, prefix, maxlen);
    }

  private:
    using EncoderFn = decltype(&encodedKey);

    Memoizer<EncoderFn> memo;
    ClassNameFn         nameGen;
    const char* const   prefix;
    const size_t        maxlen;
};

/// For variables the memoization is applied to class key type
/// \details
///    different classes get different names
template <>
struct NameGenerator<VarNameFn>
{
    NameGenerator(VarNameFn gen, const char* nameprefix, size_t numCharsOfOriginalName)
    : memo(&encodedVar), nameGen(gen), prefix(nameprefix), maxlen(numCharsOfOriginalName)
    {}

    std::string operator()(VariableKeyType key)
    {
      return memo(key, &nameGen, prefix, maxlen);
    }

  private:
    using EncoderFn = decltype(&encodedVar);

    Memoizer<EncoderFn> memo;
    VarNameFn           nameGen;
    const char* const   prefix;
    const size_t        maxlen;
};

/// For functions the memoization is applied to the actual function name
/// \details
///    to preserve overload and override relationships
template <>
struct NameGenerator<FuncNameFn>
{
    NameGenerator(FuncNameFn gen, const char* nameprefix, size_t numCharsOfOriginalName)
    : memo(&encodedName), nameGen(gen), prefix(nameprefix), maxlen(numCharsOfOriginalName)
    {}

    std::string operator()(FunctionKeyType id)
    {
      return memo(nameGen(id), prefix, maxlen);
    }

  private:
    using EncoderFn = decltype(&encodedName);

    Memoizer<EncoderFn> memo;
    FuncNameFn          nameGen;
    const char* const   prefix;
    const size_t        maxlen;
};


template <class NameGen>
NameGenerator<NameGen>
nameGenerator(NameGen gen, const char* prefix, size_t maxlen)
{
  return NameGenerator<NameGen>{gen, prefix, maxlen};
}


template <class NameGen>
NameGen createNameGenerator(NameGen defaultNamer, const char* prefix, int maxLen)
{
  if (maxLen < 0)
    return defaultNamer;

  return nameGenerator(defaultNamer, prefix, maxLen);
}


struct IncludeInOutputSet : std::unordered_set<ct::ClassKeyType>
{
  using base = std::unordered_set<ct::ClassKeyType>;
  using base::base;

  bool operator()(ct::ClassKeyType key) const
  {
    return find(key) != end();
  }
};

bool includeInOutput(const IncludeInOutputSet& outset, const ct::ClassAnalysis::value_type& elem)
{
  if (elem.second.hasVirtualTable())
    return true;

  // or any of the children is included in output
  const std::vector<ct::InheritanceDesc>&          descendants = elem.second.descendants();
  std::vector<ct::InheritanceDesc>::const_iterator aa = descendants.begin();
  std::vector<ct::InheritanceDesc>::const_iterator zz = descendants.end();

  while (aa != zz && !outset(aa->getClass()))
    ++aa;

  return aa != zz;
}


IncludeInOutputSet
buildOutputSet(const ct::ClassAnalysis& classes)
{
  IncludeInOutputSet res;

  bottomUpTraversal( classes,
                     [&res](const ct::ClassAnalysis::value_type& elem) -> void
                     {
                       if (!includeInOutput(res, elem)) return;

                       const bool success = res.insert(elem.first).second;

                       ASSERT_require(success);
                     }
                   );

  return res;
}


struct Parameters
{
  std::string dotfile_output          = opt_none;
  std::string txtfile_vtable          = opt_none;
  std::string txtfile_layout          = opt_none;
  std::string dotfile_layout          = opt_none;
  std::string txtfile_vfun            = opt_none;
  std::string txtfile_vbaseclass      = opt_none;
  int         numCharsOfOriginalName  = -1;
  bool        withOverridden          = false;
  bool        memoryPoolTraversal     = false;
  bool        checkTraversalsFindings = false;

  static const std::string dot_output;
  static const std::string txt_vtable;
  static const std::string txt_layout;
  static const std::string dot_layout;
  static const std::string txt_vfun;
  static const std::string txt_vbaseclass;
  static const std::string name_encoding;
  static const std::string vfun_overridden;
  static const std::string trav_memorypool;
  static const std::string chk_traversals;

  static const std::string opt_none;
};

const std::string Parameters::dot_output("dot");
const std::string Parameters::txt_vtable("vtable_txt");
const std::string Parameters::txt_layout("layout_txt");
const std::string Parameters::dot_layout("layout_dot");
const std::string Parameters::txt_vfun("virtual_functions");
const std::string Parameters::txt_vbaseclass("virtual_bases_txt");
const std::string Parameters::name_encoding("original_name");
const std::string Parameters::vfun_overridden("with_overriden");
const std::string Parameters::trav_memorypool("from_memorypool");
const std::string Parameters::chk_traversals("check_traversals");
const std::string Parameters::opt_none("");



struct Acuity
{
    /// sets the Acuity settings using the command line arguments
    /// \returns a list of unparsed arguments
    std::vector<std::string>
    parseArgs(std::vector<std::string> args)
    {
      scl::Parser p = Rose::CommandLine::createEmptyParserStage("", "");

      // things like --help, --version, --log, --threads, etc.
      p.with(Rose::CommandLine::genericSwitches());
      //~ p.doc("Synopsis", "@prop{programName} [@v{switches}] @v{file_names}..."); // customized synopsis

     // Create a group of switches specific to this tool
      scl::SwitchGroup acuity("Acuity - specific switches");

      acuity.name("thorn");  // the optional switch prefix

      acuity.insert(scl::Switch(Parameters::dot_output)
            .argument("filename", scl::anyParser(params.dotfile_output))
            .doc("filename for printing class hierarchy"));

      acuity.insert(scl::Switch(Parameters::txt_vtable)
            .argument("filename", scl::anyParser(params.txtfile_vtable))
            .doc("filename for printing vtable layouts (work in progress)"));

      acuity.insert(scl::Switch(Parameters::dot_layout)
            .argument("filename", scl::anyParser(params.dotfile_layout))
            .doc("filename for printing object layout tables as dot graph"));

      acuity.insert(scl::Switch(Parameters::txt_layout)
            .argument("filename", scl::anyParser(params.txtfile_layout))
            .doc("filename for printing object layout tables"));

      acuity.insert(scl::Switch(Parameters::txt_vfun)
            .argument("filename", scl::anyParser(params.txtfile_vfun))
            .doc("filename for printing virtual function information"));

      acuity.insert(scl::Switch(Parameters::txt_vbaseclass)
            .argument("filename", scl::anyParser(params.txtfile_vbaseclass))
            .doc("filename for printing virtual class construction order"));

      acuity.insert(scl::Switch(Parameters::name_encoding)
            //~ .intrinsicValue(true, params.nameEncoding)
            .argument("int", scl::anyParser(params.numCharsOfOriginalName))
            .doc("length of original name to be used in output\n(default: use original name entirely)."));

      acuity.insert(scl::Switch(Parameters::vfun_overridden, 'o')
            .intrinsicValue(true, params.withOverridden)
            .doc("lists all overriding functions in derived classes."));

      acuity.insert(scl::Switch(Parameters::trav_memorypool, 'm')
            .intrinsicValue(true, params.memoryPoolTraversal)
            .doc("obtain classes through memory pool traversal."));

      acuity.insert(scl::Switch(Parameters::chk_traversals)
            .intrinsicValue(true, params.checkTraversalsFindings)
            .doc("computes the difference between the two supported class extraction methods."));

      scl::ParserResult cmdline = p.with(acuity).parse(args).apply();

      return cmdline.unparsedArgs();
    }

    void
    writeDotFileIfRequested( const Parameters& params,
                             ct::ClassNameFn& namefn,
                             ct::ClassFilterFn include,
                             const ct::AnalysesTuple& analyses
                           );

    void
    writeObjLayoutIfRequested( const ct::CompatibilityBridge& compatLayer,
                            const Parameters& params,
                            ct::ClassNameFn& classNameFn,
                            ct::VarNameFn& varNameFn,
                            ct::ClassFilterFn include,
                            const ct::AnalysesTuple& analyses
                          );

    void
    writeVTables( const ct::CompatibilityBridge& compatLayer,
                  const Parameters& params,
                  ct::ClassNameFn& classNameFn,
                  ct::FuncNameFn& funcNameFn,
                  ct::ClassFilterFn include,
                  const ct::AnalysesTuple& analyses,
                  const ct::VirtualFunctionAnalysis& vfa
                );

    void
    writeVFunctions( const ct::CompatibilityBridge& compatLayer,
                     const Parameters& params,
                     ct::ClassNameFn& classNameFn,
                     ct::FuncNameFn& funcNameFn,
                     ct::ClassFilterFn include,
                     const ct::AnalysesTuple& analyses,
                     const ct::VirtualFunctionAnalysis& vfa
                   );

    void
    writeVFuncInfoIfRequested( const ct::CompatibilityBridge& compatLayer,
                               const Parameters& params,
                               ct::ClassNameFn& classNameFn,
                               ct::FuncNameFn& funcNameFn,
                               ct::ClassFilterFn include,
                               const ct::AnalysesTuple& analyses
                             )
    {
      const bool printVtable = (  params.txtfile_vtable != Parameters::opt_none
                               //~ || params.dotfile_vtable != Parameters::opt_none
                               );

      const bool printFunInf = (params.txtfile_vfun != Parameters::opt_none);

      if (!printVtable && !printFunInf)
        return;

      SAWYER_MESG(msgInfo())
          << "computing virtual function information"
          << std::endl;

      ct::VirtualFunctionAnalysis vfa = analyzeVirtualFunctions(compatLayer, analyses.classAnalysis());

      if (printFunInf)
        writeVFunctions(compatLayer, params, classNameFn, funcNameFn, include, analyses, vfa);

      if (printVtable)
        writeVTables(compatLayer, params, classNameFn, funcNameFn, include, analyses, vfa);
    }

    void
    writeVBaseInfoIfRequested( const ct::CompatibilityBridge& compatLayer,
                               const Parameters& params,
                               ct::ClassNameFn& classNameFn,
                               ct::ClassFilterFn include,
                               const ct::AnalysesTuple& analyses
                             );

    ct::AnalysesTuple
    invokeClassAndCastAnalyses(SgProject& n)
    {
      if (!params.memoryPoolTraversal)
        return ct::analyzeClassesAndCasts(&n);

      SAWYER_MESG(msgInfo())
                << "Using memory pool traversal; no class cast analysis." << std::endl;

      return { ct::analyzeClassesFromMemoryPool(), ct::CastAnalysis{} };
    }

    void printClassesDifferenceFoundByTraversal(SgProject& n)
    {
      using ClassSets = std::set<ClassKeyType>;

      if (!params.checkTraversalsFindings) return;

      ct::ClassAnalysis fromAst  = ct::analyzeClasses(&n);
      ct::ClassAnalysis fromPool = ct::analyzeClassesFromMemoryPool();

      ClassSets         astClasses;
      ClassSets         poolClasses;
      auto              extractClass =
          [](ct::ClassAnalysis::value_type& el) -> const SgClassDefinition*
          {
            return el.first;
          };

      std::transform( fromAst.begin(),  fromAst.end(),
                      std::inserter(astClasses, astClasses.begin()),
                      extractClass
                    );
      std::transform( fromPool.begin(), fromPool.end(),
                      std::inserter(poolClasses, poolClasses.begin()),
                      extractClass
                    );

      ct::ClassNameFn           nameClasses = ct::CompatibilityBridge{}.classNaming();
      std::vector<ClassKeyType> diff;

      std::set_difference( astClasses.begin(),  astClasses.end(),
                           poolClasses.begin(), poolClasses.end(),
                           std::back_inserter(diff)
                         );

      if (!diff.empty())
        msgWarn() << "The following classes are not found by the pool traversal: ";

      for (ClassKeyType key : diff)
        msgWarn() << "\n  - " << nameClasses(key) << ct::dbgInfo(key)
                  << std::endl;

      diff.clear();

      std::set_difference( poolClasses.begin(), poolClasses.end(),
                           astClasses.begin(),  astClasses.end(),
                           std::back_inserter(diff)
                         );

      if (!diff.empty())
        msgWarn() << "The following classes are not found by the AST traversal: ";

      for (ClassKeyType key : diff)
        msgWarn() << "\n  - " << nameClasses(key) << dbgInfo(key, 6)
                  << std::endl;
    }

    // legacy interface
    void process(SgProject& project, ct::VariableIdMapping&, const ct::FunctionIdMapping&)
    {
      process(project);
    }

    void process(SgProject& project)
    {
      SAWYER_MESG(msgInfo())
                << "Thorn 2: "
                << params.dotfile_output
                << " - " << params.txtfile_layout
                << std::endl;

      printClassesDifferenceFoundByTraversal(project);

      SAWYER_MESG(msgInfo())
                << "getting all classes.. " << std::endl;

      ct::CompatibilityBridge compatLayer;
      ct::AnalysesTuple       analyses = invokeClassAndCastAnalyses(project);

      const ct::ClassAnalysis& allClasses = analyses.classAnalysis();
      const int           numClasses = allClasses.size();
      SAWYER_MESG(msgInfo())
                << "getting all (" << numClasses << ") structs done. " << std::endl;

      IncludeInOutputSet  outset = buildOutputSet(allClasses);
      SAWYER_MESG(msgInfo())
                << "number of classes with virtual tables: " << outset.size() << std::endl;

      const int           maxlen = params.numCharsOfOriginalName;
      ct::ClassNameFn     clsNameGen = createNameGenerator(compatLayer.classNaming(), "Cl", maxlen);
      ct::FuncNameFn      funNameGen = createNameGenerator(compatLayer.functionNaming(), "fn", maxlen);
      ct::VarNameFn       varNameGen = createNameGenerator(compatLayer.variableNaming(), "var", maxlen);

      writeDotFileIfRequested  (params, clsNameGen, outset, analyses);
      writeObjLayoutIfRequested(compatLayer, params, clsNameGen, varNameGen, outset, analyses);
      writeVFuncInfoIfRequested(compatLayer, params, clsNameGen, funNameGen, outset, analyses);
      writeVBaseInfoIfRequested(compatLayer, params, clsNameGen, outset, analyses);
    }

  private:
    Parameters params;
};




void
Acuity::writeObjLayoutIfRequested( const ct::CompatibilityBridge& compatLayer,
                                   const Parameters& params,
                                   ct::ClassNameFn& classNameFn,
                                   ct::VarNameFn& varNameFn,
                                   ct::ClassFilterFn include,
                                   const ct::AnalysesTuple& analyses
                                 )
{
  using OutputGenFn = decltype(&classLayoutDot);

  const bool printLayout = (  params.txtfile_layout != Parameters::opt_none
                           || params.dotfile_layout != Parameters::opt_none
                           );

  if (!printLayout)
    return;

  SAWYER_MESG(msgInfo())
            << "computing class layout"
            << std::endl;

  ct::ObjectLayoutContainer layouts = ct::computeObjectLayouts(analyses.classAnalysis());

  auto outputGen =
    [&](const std::string& filename, const std::string& filekind, OutputGenFn outfn) -> void
    {
      if (filename == Parameters::opt_none) return;

      SAWYER_MESG(msgInfo())
                << "writing class layout file (" << filekind << "):" << filename << ".."
                << std::endl;

      std::ofstream outfile{filename};

      outfn(outfile, classNameFn, varNameFn, include, layouts);

      SAWYER_MESG(msgInfo())
                 << "done writing layout." << std::endl;
    };

  outputGen(params.txtfile_layout, "txt", classLayoutTxt);
  outputGen(params.dotfile_layout, "dot", classLayoutDot);
}


void
Acuity::writeVTables( const ct::CompatibilityBridge& compatLayer,
                      const Parameters& params,
                      ct::ClassNameFn& classNameFn,
                      ct::FuncNameFn& funcNameFn,
                      ct::ClassFilterFn include,
                      const ct::AnalysesTuple& analyses,
                      const ct::VirtualFunctionAnalysis& vfa
                    )
{
  using OutputGenFn = decltype(&vtableLayoutTxt);

  SAWYER_MESG(msgInfo())
            << "computing vtable layout"
            << std::endl;

  ct::VTableLayoutContainer layouts = ct::computeVTableLayouts(analyses.classAnalysis(), vfa, compatLayer);

  auto outputGen =
    [&](const std::string& filename, const std::string& filekind, OutputGenFn outfn) -> void
    {
      if (filename == Parameters::opt_none) return;

      SAWYER_MESG(msgInfo())
                << "writing vtable layout file (" << filekind << "):" << filename << ".."
                << std::endl;

      std::ofstream outfile{filename};

      outfn(outfile, classNameFn, funcNameFn, include, layouts);

      SAWYER_MESG(msgInfo())
                << "done writing layout." << std::endl;
    };

  outputGen(params.txtfile_vtable, "txt", vtableLayoutTxt);
  //~ outputGen(params.dotfile_vtable, "dot", vtableLayoutDot);
}





void
Acuity::writeVFunctions( const ct::CompatibilityBridge& compatLayer,
                         const Parameters& params,
                         ct::ClassNameFn& classNameFn,
                         ct::FuncNameFn& funcNameFn,
                         ct::ClassFilterFn include,
                         const ct::AnalysesTuple& analyses,
                         const ct::VirtualFunctionAnalysis& vfa
                       )
{
  SAWYER_MESG(msgInfo())
            << "writing virtual function information file " << params.txtfile_vfun << ".."
            << std::endl;

  std::ofstream outfile{params.txtfile_vfun};

  virtualFunctionsTxt( outfile,
                       classNameFn,
                       funcNameFn,
                       include,
                       analyses.classAnalysis(),
                       vfa,
                       params.withOverridden
                     );

  SAWYER_MESG(msgInfo())
            << "writing virtual function information done" << std::endl;
}

void
Acuity::writeVBaseInfoIfRequested( const ct::CompatibilityBridge& compatLayer,
                                   const Parameters& params,
                                   ct::ClassNameFn& classNameFn,
                                   ct::ClassFilterFn include,
                                   const ct::AnalysesTuple& analyses
                                 )
{
  if (params.txtfile_vbaseclass == Parameters::opt_none)
    return;

  msgInfo() << "writing virtual base class initialization order file " << params.txtfile_vbaseclass << ".."
            << std::endl;

  std::ofstream outfile{params.txtfile_vbaseclass};

  virtualBaseClassInitOrderTxt( outfile,
                                classNameFn,
                                include,
                                analyses.classAnalysis()
                              );

  msgInfo() << "writing virtual base class initialization order file done" << std::endl;
}


void
Acuity::writeDotFileIfRequested( const Parameters& params,
                                 ct::ClassNameFn& classNameFn,
                                 ct::ClassFilterFn include,
                                 const ct::AnalysesTuple& analyses
                               )
{
  if (params.dotfile_output == Parameters::opt_none)
    return;

  msgInfo() << "writing dot file " << params.dotfile_output << ".."
            << std::endl;

  std::ofstream outfile{params.dotfile_output};

  classHierarchyDot( outfile,
                     classNameFn,
                     include,
                     analyses.classAnalysis(),
                     analyses.castAnalysis()
                   );

  msgInfo() << "writing dot file done" << std::endl;
}


namespace
{
  struct CStringVector : private std::vector<char*>
  {
      using base = std::vector<char*>;

      CStringVector()
      : base()
      {}

      explicit
      CStringVector(const std::vector<std::string>& args)
      : base()
      {
        base::reserve(args.size());

        for (const std::string& s : args)
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
  constexpr bool LEGACY_MODE = false;

  using Sawyer::Message::mfacilities;
  //~ using GuardedVariableIdMapping = std::unique_ptr<ct::VariableIdMappingExtended>;

  int errorCode = 1;

  try
  {
    ROSE_INITIALIZE;
    ct::CodeThornLib::configureRose();

    std::vector<std::string> cmdLineArgs{argv+0, argv+argc};
    Acuity                   acuity;
    CStringVector            unparsedArgsCStyle(acuity.parseArgs(std::move(cmdLineArgs)));
    int                      thornArgc = unparsedArgsCStyle.size();
    char**                   thornArgv = unparsedArgsCStyle.firstCArg();

    //~ for (int i = 0; i < thornArgc; ++i)
      //~ std::cerr << thornArgv[i] << std::endl;

    CodeThornOptions         ctOpt;
    //~ LTLOptions               ltlOpt;    // to be moved into separate tool
    //~ ParProOptions            parProOpt; // options only available in parprothorn

    //~ parseCommandLine(thornArgc, thornArgv, logger, thorn2version, ctOpt, ltlOpt, parProOpt);
    mfacilities.control(ctOpt.logLevel);
    msgTrace() << "Log level is " << ctOpt.logLevel << endl;
    ct::TimingCollector      tc;

    //~ tc.startTimer();
    SgProject* project = ct::CodeThornLib::runRoseFrontEnd(thornArgc,thornArgv,ctOpt,tc);
    ASSERT_not_null(project);
    //~ tc.stopTimer();

    msgTrace() << "Parsing and creating AST finished."<<endl;

    if (LEGACY_MODE)
    {
      ct::FunctionIdMapping funMap;
      ct::VariableIdMapping varMap;

      funMap.computeFunctionSymbolMapping(project);
      varMap.computeVariableSymbolMapping(project);

      acuity.process(*project, varMap, funMap);
    }
    else
    {
      acuity.process(*project);
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

