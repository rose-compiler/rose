// C++ code calling an Ada function.

// #include <rose_paths.h>
// #include <rose.h>

// DQ (11/13/2017): This is a violation, sage3basic.h must be the first file included.
// #include "rose_config.h"

#include "sage3basic.h"
#include "rose_config.h"

#include "CommandLine.h"
#include "Sawyer/CommandLine.h"


#include <boost/filesystem.hpp>

#include "ada_support.h"

// using namespace std;

// #include <stdio.h>
#include "a_nodes.h"
#include "tool_2_wrapper.h"

// extern "C" void dot_asisinit (void);
// extern "C" void dot_asisfinal (void);

namespace boostfs = boost::filesystem;
namespace scl     = Sawyer::CommandLine;

namespace Ada_ROSE_Translation
{
  Sawyer::Message::Facility mlog;

  struct Settings
  {
    bool processPredefinedUnits = false;
    bool processImplementationUnits = false;
    bool asisDebug = false;
  };

}


#ifdef BUILD_EXECUTABLE
int main(int argc, char** argv)
   {
     const std::vector<std::string> args(argv, argv + argc);
#else
  int ada_main(const std::vector<std::string>& args, SgSourceFile* file)
   {
#endif
     using Ada_ROSE_Translation::mlog;

     ROSE_INITIALIZE;

     int status = 0;

     mlog = Sawyer::Message::Facility("Ada2ROSE", Rose::Diagnostics::destination);
     mprintf ("In ada_support.C: In ada_main(): calling ada support for file = %s \n",file->getFileName().c_str());

     Ada_ROSE_Translation::Settings settings;

     scl::Parser p = Rose::CommandLine::createEmptyParserStage("", "");

     p.errorStream(mlog[Sawyer::Message::FATAL]);               // print messages and exit rather than throwing exceptions
     //~ p.with(CommandLine::genericSwitches());   // things like --help, --version, --log, --threads, etc.
     //~ p.doc("Synopsis", "@prop{programName} [@v{switches}] @v{file_names}..."); // customized synopsis

     // Create a group of switches specific to this tool
     scl::SwitchGroup ada2Rose("Ada2ROSE (A2R) - specific switches");

     ada2Rose.name("asis");  // the optional switch prefix

     ada2Rose.insert(scl::Switch("process_predefined_units")
           .intrinsicValue(true, settings.processPredefinedUnits)
           .doc("With Asis's predefined units"));

     ada2Rose.insert(scl::Switch("process_implementation_units")
           .intrinsicValue(true, settings.processImplementationUnits)
           .doc("Enables Asis implementation unit processing"));

     ada2Rose.insert(scl::Switch("asis_debug")
           .intrinsicValue(true, settings.asisDebug)
           .doc("Sets Asis debug flag"));

     p.with(ada2Rose).parse(args).apply();

  // char *gnat_home   = "/usr/workspace/wsb/charles/bin/adacore/gnat-gpl-2017-x86_64-linux";
     const char *gnat_home   = std::getenv("GNAT_HOME");

     if (!gnat_home) gnat_home = "/home/quinlan1/ROSE/ADA/x86_64-linux/adagpl-2017/gnatgpl/gnat-gpl-2017-x86_64-linux-bin";

     //~ std::cerr << "Settings: processPredefinedUnits = " << settings.processPredefinedUnits
               //~ << ", processImplementationUnits = " << settings.processImplementationUnits
               //~ << ", asisDebug = " << settings.asisDebug
               //~ << std::endl;
     mprintf ("BEGIN.\n");

     Nodes_Struct head_nodes;

#if 1
     {
       typedef boostfs::path::string_type string_type;

    // DQ (9/15/2017): Updated to include output directory.
    // PP (10/31/20): Produce Ada temp+obj files in src-file specific directory.
       boostfs::path currentDir    = boostfs::current_path();
       string_type   srcFile       = file->getFileName();
       string_type   gnatOutputDir = currentDir.string<string_type>();

       gnatOutputDir += boostfs::path::preferred_separator;
       gnatOutputDir += "gnatOutput";

       // create a new output directory for every import file to support
       // parallel compilation (e.g., testing).
       boostfs::create_directory(gnatOutputDir);

       size_t pos = srcFile.rfind(boostfs::path::preferred_separator);

       if (pos == string_type::npos) pos = 0;

       gnatOutputDir += boostfs::path::preferred_separator;
       gnatOutputDir += srcFile.substr(pos);
       gnatOutputDir += "-obj";

       boostfs::create_directory(gnatOutputDir);
       boostfs::current_path(gnatOutputDir);

       char* cstring_SrcFile = const_cast<char*>(srcFile.c_str());
       char* cstring_GnatOutputDir = const_cast<char*>(gnatOutputDir.c_str());

    // DQ (31/8/2017): Definitions of these functions still need to be provided to via libraries to be able to link ROSE.
       dot_asisinit();

    // PP (11/5/20): Use Charles' new tool_2_wrapper_with_flags function
       mprintf( "calling Asis: src:%s gnat:%s outdir:%s pdunit:%d implunit:%d dbg:%d\n",
                cstring_SrcFile, gnat_home, cstring_GnatOutputDir,
                settings.processPredefinedUnits, settings.processImplementationUnits, settings.asisDebug
              );

       head_nodes = tool_2_wrapper_with_flags( cstring_SrcFile,
                                               const_cast<char*>(gnat_home),
                                               cstring_GnatOutputDir,
                                               settings.processPredefinedUnits,
                                               settings.processImplementationUnits,
                                               settings.asisDebug
                                             );

       if (head_nodes.Elements == NULL) {
          mprintf ("tool_2_wrapper_with_flags returned NO elements.\n");
          status = 1;
       } else {
          mprintf ("tool_2_wrapper_with_flags returned %i elements.\n", head_nodes.Elements->Next_Count + 1);
       }

       boostfs::current_path(currentDir);
     }
#endif

     mprintf ("END.\n");

     Ada_ROSE_Translation::ada_to_ROSE_translation(head_nodes, file);

     dot_asisfinal();
     mprintf ("Leaving ada_main(): status = %d \n", status);
     return status;
   }



