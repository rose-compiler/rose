// C++ code calling an Ada function.

#include "sage3basic.h"
#include "rose_config.h"

#include "cmdline.h"
#include "Rose/CommandLine.h"
#include "Sawyer/CommandLine.h"
#include "processSupport.h"

#include <boost/filesystem.hpp>

#include "ada_support.h"

#include "a_nodes.h"
#include "adapter_wrapper.h"
#include "FileUtility.h"

namespace boostfs = boost::filesystem;
namespace scl     = Sawyer::CommandLine;
namespace sas     = Sawyer::Assert;


// minimal declarations from Ada_to_ROSE.h
namespace Ada_ROSE_Translation
{
  Sawyer::Message::Facility mlog;

  /// initialize translation settins
  void initialize(const Rose::Cmdline::Ada::CmdlineSettings& settings);
}

#if OBSOLETE_CODE
namespace
{
  bool eq( const Rose::Cmdline::Ada::CmdlineSettings& lhs,
           const Rose::Cmdline::Ada::CmdlineSettings& rhs
         )
  {
    return (  lhs.processPredefinedUnits == rhs.processPredefinedUnits
           && lhs.processImplementationUnits == rhs.processImplementationUnits
           && lhs.failhardAdb == rhs.failhardAdb
           && lhs.asisDebug == rhs.asisDebug
           && lhs.logWarn == rhs.logWarn
           && lhs.logInfo == rhs.logInfo
           && lhs.logTrace == rhs.logTrace
           );
  }
}
#endif /* OBSOLETE_CODE */

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

     Rose::Cmdline::Ada::CmdlineSettings settings = Rose::Cmdline::Ada::commandlineSettings();
     std::vector<std::string> unparsedArgs = args;

     std::string              ASISIncludeArgs;
     std::string              GNATArgs;
     std::string              ASISArgs;
     std::vector<std::string> includePaths;

     for (unsigned int i=1; i < unparsedArgs.size(); ++i)
        {
       // most options appear as -<option>
       // have to process +w2 (warnings option) on some compilers so include +<option>
          if ( unparsedArgs[i].size() >= 2 && (unparsedArgs[i][0] == '-') && (unparsedArgs[i][1] == 'I') )
             {
               std::string includeDirectorySpecifier =  unparsedArgs[i].substr(2);
               includeDirectorySpecifier = Rose::StringUtility::getAbsolutePathFromRelativePath(includeDirectorySpecifier );
               includePaths.push_back(includeDirectorySpecifier);
             }
          else if (unparsedArgs[i].find("-gnat") != string::npos)
             {
                GNATArgs.append(unparsedArgs[i] + " ");
             }
        }

     for (vector<string>::const_iterator i = includePaths.begin(); i != includePaths.end(); ++i)
        {
          ASISIncludeArgs.append("-I" + *i + " ");
        }
//     if (includePaths.size() != 0)
//        ASISIncludeArgs = ASISIncludeArgs.substr(0, ASISIncludeArgs.length()-1);

     ASISArgs = GNATArgs + ASISIncludeArgs;

     if(!ASISArgs.empty() && ASISArgs.at(ASISArgs.length()-1) == ' ')
        ASISArgs = ASISArgs.substr(0, ASISArgs.length()-1);

     std::string warninglevels = "none, error, fatal";
     Sawyer::Message::Facilities logctrl;

     logctrl.insert(mlog);
     //~ logctrl.control("none, error, warn, fatal");

     if (settings.logWarn)  warninglevels += ", warn";
     if (settings.logTrace) warninglevels += ", trace";
     if (settings.logInfo)  warninglevels += ", info";

     logctrl.control(warninglevels);

     mlog[Sawyer::Message::TRACE] << "In ada_support.C: In ada_main(): calling ada support for file = "
                                  << file->getFileName()
                                  << std::endl;

     const char* gnat_home = std::getenv("GNAT_HOME");

     if (!gnat_home)
     {
       mlog[Sawyer::Message::FATAL] << "Environment variable GNAT_HOME is not set.\n"
                                    << "  Aborting ROSE.."
                                    << std::endl;

       return 1;
     }

     // set ROSE assertion behavior to throw, which seems more robust in the context of
     //   the Asis frontend.
     sas::AssertFailureHandler roseFailureHandler = Rose::failedAssertionBehavior();

     Rose::failedAssertionBehavior(Rose::throwOnFailedAssertion);

     mlog[Sawyer::Message::TRACE] << "BEGIN." << std::endl;

     Nodes_Struct head_nodes;

     {
       typedef boostfs::path::string_type string_type;

    // DQ (9/15/2017): Updated to include output directory.
    // PP (10/31/20): Produce Ada temp+obj files in src-file specific directory.
       boostfs::path currentDir    = boostfs::current_path();
       string_type   srcFile       = file->getFileName();
       string_type   gnatOutputDir = currentDir.string<string_type>();
       string_type   defaultConfig = currentDir.string<string_type>();

       gnatOutputDir += boostfs::path::preferred_separator;
       gnatOutputDir += "gnatOutput";

       // check if default configuation, gnat.adc, exists
       defaultConfig += boostfs::path::preferred_separator;
       defaultConfig += "gnat.adc";
       struct stat buffer;
       // inject gnatec only when gnat.adc is available, and gnatA switch is not used
       if (stat (defaultConfig.c_str(), &buffer) == 0 && GNATArgs.find("gnatA") == std::string::npos )
       {
         if(!ASISArgs.empty())
            ASISArgs += " ";
         ASISArgs += "-gnatec=";
         ASISArgs += defaultConfig;
       }

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
       char* cstring_Args = const_cast<char*>(ASISArgs.c_str());
       char* cstring_GnatOutputDir = const_cast<char*>(gnatOutputDir.c_str());

    // DQ (31/8/2017): Definitions of these functions still need to be provided to via libraries to be able to link ROSE.
       asis_adapterinit();

    // PP (11/5/20): Use Charles' new adapter_wrapper_with_flags function
       mlog[Sawyer::Message::TRACE] << "calling Asis: src: " << cstring_SrcFile
                                    << " gnat: " << gnat_home
                                    << " outdir: " << cstring_GnatOutputDir
                                    << " pdunit: " << settings.processPredefinedUnits
                                    << " implunit: " << settings.processImplementationUnits
                                    << " dbg: " << settings.asisDebug
                                    << std::endl;

       head_nodes = adapter_wrapper_with_flags( cstring_SrcFile,
                                                const_cast<char*>(gnat_home),
                                                cstring_Args,
                                                cstring_GnatOutputDir,
                                                settings.processPredefinedUnits,
                                                settings.processImplementationUnits,
                                                settings.asisDebug
                                              );

       if (head_nodes.Elements == nullptr) {
          mlog[Sawyer::Message::FATAL] << "adapter_wrapper_with_flags returned NULL." << std::endl;
          status = 1;
       } else {
          mlog[Sawyer::Message::INFO] << "adapter_wrapper_with_flags returned "
                                      << head_nodes.Elements->Next_Count + 1
                                      << " elements."
                                      << std::endl;
       }

       boostfs::current_path(currentDir);
     }

     mlog[Sawyer::Message::TRACE] << "END." << std::endl;

     try
     {
       Ada_ROSE_Translation::initialize(settings);
       Ada_ROSE_Translation::ada_to_ROSE_translation(head_nodes, file);
     }
     catch (const std::runtime_error& e)
     {
       mlog[Sawyer::Message::FATAL] << "caught runtime_error: " << typeid(e).name() << " " << e.what() << std::endl;
       status = 1;
     }
     catch (const std::logic_error& e)
     {
       mlog[Sawyer::Message::FATAL] << "caught logic_error: " << typeid(e).name() << " " << e.what() << std::endl;
       status = 1;
     }
     catch (...)
     {
       mlog[Sawyer::Message::FATAL] << "An unexpected exception terminated the ASIS to ROSE converter" << std::endl;
       status = 1;
     }

     asis_adapterfinal();

     // restore ROSE assertion behavior
     Rose::failedAssertionBehavior(roseFailureHandler);
     mlog[Sawyer::Message::TRACE] << "Leaving ada_main(): status = " << status << std::endl;
     return status;
   }



