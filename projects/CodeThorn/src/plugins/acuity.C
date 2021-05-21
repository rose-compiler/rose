// An example ROSE plugin
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include <boost/lexical_cast.hpp>

// Mandatory include headers
#include "rose.h"
#include "plugin.h"

#include "Rose/CommandLine.h"
#include "Sawyer/CommandLine.h"

// Programmatic headers
//~ #include "ClassHierarchyGraph.h"
#include "VariableIdMapping.h"
#include "sageGeneric.h"

#include "ClassHierarchyAnalysis.h"
#include "ObjectLayoutAnalysis.h"
#include "ClassHierarchyWriter.h"

namespace ct  = CodeThorn;
namespace si  = SageInterface;
namespace scl = Sawyer::CommandLine;

namespace
{
  std::string
  classNameGenerator(const ct::ClassKeyType key)
  {
    static constexpr size_t MAXLEN = 3;

    static int uniquenum = 0;

    std::stringstream buf;
    std::string       origname = ct::typeNameOfClassKeyType(key);
    size_t            len = std::min(MAXLEN, origname.length());

    buf << (MAXLEN ? origname.substr(0, len) : std::string{"Cl"})
        << (++uniquenum);

    return buf.str();
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
    if (ct::hasVirtualTable(elem))
      return true;

    // or any of the children is included in output
    const std::vector<ct::InheritanceDesc>&          descendants = elem.second.children();
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

                         ROSE_ASSERT(success);
                       }
                     );

    return res;
  }

// \brief handlers to skip template processing

  struct Settings
  {
    bool processPredefinedUnits = true;
    bool processImplementationUnits = true;
    bool asisDebug = false;
    bool logTrace  = false;
    bool logInfo   = false;
    bool logWarn   = false;
  };


struct Parameters
{
  std::string dotfile_orig   = opt_none;
  std::string dotfile_code   = opt_none;
  std::string txtfile_layout = opt_none;

  static const std::string dot_orig;
  static const std::string dot_code;
  static const std::string txt_layout;

  static const std::string opt_none;
};

const std::string Parameters::dot_orig("dot");
const std::string Parameters::dot_code("dot_enc");
const std::string Parameters::txt_layout("txt_layout");
const std::string Parameters::opt_none("");


} // anonymous namespace


struct Acuity : Rose::PluginAction
{
    typedef Rose::PluginAction base;

    // passed as -rose:plugin_arg_NAME
    //   e.g., -rose:plugin_arg_o output file
    bool ParseArgs(const std::vector<std::string>& args) ROSE_OVERRIDE
    {
      scl::Parser p = Rose::CommandLine::createEmptyParserStage("", "");

      //~ p.errorStream(mlog[Sawyer::Message::FATAL]);               // print messages and exit rather than throwing exceptions
      p.with(Rose::CommandLine::genericSwitches());   // things like --help, --version, --log, --threads, etc.
      //~ p.doc("Synopsis", "@prop{programName} [@v{switches}] @v{file_names}..."); // customized synopsis

     // Create a group of switches specific to this tool
      scl::SwitchGroup acuity("Acuity - specific switches");

      acuity.name("acuity");  // the optional switch prefix

      acuity.insert(scl::Switch(Parameters::dot_orig)
            .argument("filename", scl::anyParser(params.dotfile_orig))
            .doc("filename for printing class hierarchy"));

      acuity.insert(scl::Switch(Parameters::dot_code)
            .argument("filename", scl::anyParser(params.dotfile_code))
            .doc("filename for printing class hierarchy (encoded names)"));

      acuity.insert(scl::Switch(Parameters::txt_layout)
            .argument("filename", scl::anyParser(params.txtfile_layout))
            .doc("filename for printing object layout tables"));

      p.with(acuity).parse(args).apply();

      return base::ParseArgs(args);
    }

    void
    writeDotFileIfRequested( const std::string& filename,
                             ct::NameFn namefn,
                             const IncludeInOutputSet& outset,
                             const ct::AnalysesTuple& analyses
                           );

    void
    writeObjLayoutIfRequested(const std::string& filename, const ct::ClassAnalysis& analysis, SgProject*);


    void process(SgProject* n) ROSE_OVERRIDE
    {
      ROSE_ASSERT(n);
      logInfo() << "Acuity: "
                << params.dotfile_orig
                << " - " << params.dotfile_code
                << " - " << params.txtfile_layout
                << std::endl;

      logInfo() << "getting all classes.. " << std::endl;
      ct::AnalysesTuple analyses = ct::extractFromProject(n);
      logInfo() << "getting all classes done. " << std::endl;

      IncludeInOutputSet outset = buildOutputSet(std::get<0>(analyses));

      writeDotFileIfRequested  (params.dotfile_orig,   ct::typeNameOfClassKeyType, outset, analyses);
      writeDotFileIfRequested  (params.dotfile_code,   classNameGenerator,         outset, analyses);
      writeObjLayoutIfRequested(params.txtfile_layout, std::get<0>(analyses), n);
    }

  private:
    Parameters params;
};


void
Acuity::writeObjLayoutIfRequested(const std::string& filename, const ct::ClassAnalysis& classes, SgProject* prj)
{
  if (filename == Parameters::opt_none)
    return;

  logInfo() << "computing class layout"
            << std::endl;

  ct::VariableIdMapping       vmap;

  vmap.computeVariableSymbolMapping(prj, 99);

  ct::RoseCompatibilityBridge astctx{vmap};
  ct::ObjectLayoutContainer   layouts = computeObjectLayouts(astctx, classes);

  logInfo() << "writing class layout file " << filename << ".."
            << std::endl;

  std::ofstream                      outfile{filename};

  outfile << ct::ObjectLayoutPrinter{astctx, layouts} << std::endl;

  logInfo() << "writing layout file done" << std::endl;
}


void
Acuity::writeDotFileIfRequested( const std::string& filename,
                                 ct::NameFn namefn,
                                 const IncludeInOutputSet& outset,
                                 const ct::AnalysesTuple& analyses
                               )
{
  if (filename == Parameters::opt_none)
    return;

  logInfo() << "writing dot file " << filename << ".."
            << std::endl;

  std::ofstream outfile{filename};

  write(outfile, namefn, outset, std::get<0>(analyses), std::get<1>(analyses));

  logInfo() << "writing dot file done" << std::endl;
}

//Step 2: Declare a plugin entry with a unique name
//        Register it under a unique action name plus some description
static Rose::PluginRegistry::Add<Acuity> vcallName("Acuity", "Class Hierarchy Analysis Tool.");


