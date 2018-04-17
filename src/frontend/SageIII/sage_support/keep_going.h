#ifndef ROSE_KEEPGOING_H
#define ROSE_KEEPGOING_H

/**
 * \file    signal_handling.h
 * \author  Justin Too <too1@llnl.gov>
 * \date    November 16, 2013
 */

#include <setjmp.h>
#include <signal.h>

#include <assert.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h> //getpid()
#include <unistd.h>    //getpid()

#include <iostream>
#include <map>
#include <string>
// Forward declarations

class SgFile;
class SgProject;

namespace Rose {

  namespace KeepGoing {
  //-----------------------------------------------------------------------------
  // Global Data
  //-----------------------------------------------------------------------------
  extern bool g_keep_going;
  extern struct sigaction SignalAction;
  typedef void(*SignalHandlerFunction)(int);
  
  // Often we run a rose translator/analyzer to process a set of input files.
  // We use this to store the results for each file.
  // Later, the generated report for successfully processed file will contain the results. 
  // So in the end, the success report will contain both the file names and corresponding analysis results.
  extern std::map<SgFile* , std::string> File2StringMap; 
  
  //-----------------------------------------------------------------------------
  // Public API
  //-----------------------------------------------------------------------------
  #ifndef _MSC_VER
      #define KEEP_GOING_CAUGHT_COMMANDLINE_SIGNAL                                \
        (                                                                         \
            Rose::KeepGoing::g_keep_going &&                                      \
            Rose::KeepGoing::set_signal_handler(                                  \
                &Rose::KeepGoing::Frontend::Commandline::SignalHandler) &&        \
            sigsetjmp(Rose::KeepGoing::Frontend::Commandline::jmp_target, 0) != 0 \
        )
      #define KEEP_GOING_CAUGHT_FRONTEND_SIGNAL                         \
        (                                                               \
            Rose::KeepGoing::g_keep_going &&                            \
            Rose::KeepGoing::set_signal_handler(                        \
                &Rose::KeepGoing::Frontend::SignalHandler) &&           \
            sigsetjmp(Rose::KeepGoing::Frontend::jmp_target, 0) != 0    \
        )
      #define KEEP_GOING_CAUGHT_FRONTEND_SECONDARY_PASS_SIGNAL                        \
        (                                                                             \
            Rose::KeepGoing::g_keep_going &&                                          \
            Rose::KeepGoing::set_signal_handler(                                      \
                &Rose::KeepGoing::Frontend::SecondaryPass::SignalHandler) &&          \
            sigsetjmp(Rose::KeepGoing::Frontend::SecondaryPass::jmp_target, 0) != 0   \
        )
      #define KEEP_GOING_CAUGHT_MIDEND_SIGNAL                         \
        (                                                             \
            Rose::KeepGoing::g_keep_going &&                          \
            Rose::KeepGoing::set_signal_handler(                      \
                &Rose::KeepGoing::Midend::SignalHandler) &&           \
            sigsetjmp(Rose::KeepGoing::Midend::jmp_target, 0) != 0    \
        )
      #define KEEP_GOING_CAUGHT_BACKEND_UNPARSER_SIGNAL                       \
        (                                                                     \
            Rose::KeepGoing::g_keep_going &&                                  \
            Rose::KeepGoing::set_signal_handler(                              \
                &Rose::KeepGoing::Backend::Unparser::SignalHandler) &&        \
            sigsetjmp(Rose::KeepGoing::Backend::Unparser::jmp_target, 0) != 0 \
        )
      #define KEEP_GOING_CAUGHT_BACKEND_COMPILER_SIGNAL                       \
        (                                                                     \
            Rose::KeepGoing::g_keep_going &&                                  \
            Rose::KeepGoing::set_signal_handler(                              \
                &Rose::KeepGoing::Backend::Compiler::SignalHandler) &&        \
            sigsetjmp(Rose::KeepGoing::Backend::Compiler::jmp_target, 0) != 0 \
        )
  #else //_MSC_VER
      // TOO1 (2013/12/30): See setjmp/longjmp for implementation details:
      //
      //                     http://msdn.microsoft.com/en-us/library/xe7acxfb.aspx
      //                     http://msdn.microsoft.com/en-us/library/3ye15wsy.aspx
      //    See:
      //                     http://stackoverflow.com/questions/8934879/how-to-handle-sigabrt-signal
      //
      #define KEEP_GOING_CAUGHT_COMMANDLINE_SIGNAL false
      #define KEEP_GOING_CAUGHT_FRONTEND_SIGNAL false
      #define KEEP_GOING_CAUGHT_FRONTEND_SECONDARY_PASS_SIGNAL false
      #define KEEP_GOING_CAUGHT_MIDEND_SIGNAL false
      #define KEEP_GOING_CAUGHT_BACKEND_UNPARSER_SIGNAL false
      #define KEEP_GOING_CAUGHT_BACKEND_COMPILER_SIGNAL false
  #endif //_MSC_VER
  
  //-----------------------------------------------------------------------------
  // Private API
  //-----------------------------------------------------------------------------
  bool set_signal_handler(SignalHandlerFunction handler);
  
  namespace Frontend {
  #ifndef _MSC_VER
    extern sigjmp_buf jmp_target;
  #endif //_MSC_VER
    void SignalHandler(int sig);
  
    namespace Commandline {
      #ifndef _MSC_VER
        extern sigjmp_buf jmp_target;
      #endif //_MSC_VER
      void SignalHandler(int sig);
    }// ::Rose::KeepGoing::Frontend::Commandline
  
    namespace SecondaryPass {
      #ifndef _MSC_VER
        extern sigjmp_buf jmp_target;
      #endif //_MSC_VER
      void SignalHandler(int sig);
    }// ::Rose::KeepGoing::Frontend::SecondaryPass
  }// ::Rose::KeepGoing::Frontend
  
  namespace Midend {
    #ifndef _MSC_VER
      extern sigjmp_buf jmp_target;
    #endif //_MSC_VER
      void SignalHandler(int sig);
  }// ::Rose::KeepGoing::Midend
  
  namespace Backend {
    bool UseOriginalInputFile(SgFile* project);
    
    namespace Unparser {
      #ifndef _MSC_VER
        extern sigjmp_buf jmp_target;
      #endif //_MSC_VER
      void SignalHandler(int sig);
    }// ::Rose::KeepGoing::Backend::Unparser
    
    namespace Compiler {
      #ifndef _MSC_VER
        extern sigjmp_buf jmp_target;
      #endif //_MSC_VER
      void SignalHandler(int sig);
    }// ::Rose::KeepGoing::Backend::Compiler
    
    }// ::Rose::KeepGoing::Backend
  
    // Helper functions to create translators with keep-going handling
    void
    ShowUsage(std::string program_name);

   extern  bool verbose; 
   extern  bool enable_ast_tests; 
   extern std::string report_filename__fail;
   extern std::string report_filename__pass;
   extern std::string expectations_filename__fail;
   extern std::string expectations_filename__pass;
   extern std::string path_prefix;
 
    /**
     * @returns The current time in the specified format
     */
    std::string
    GetTimestamp(const std::string& format = "%Y-%m-%d %H:%M:%S");
    
    std::string
    StripPrefix(const std::string& prefix, const std::string& str);
    
    /**
     * @brief Append msg to a file 
     */
    void
    AppendToFile(const std::string& filename, const std::string& msg);
    
    /**
     * @returns A map of all filenames expected to fail.
     */
    std::map<std::string, std::string>
    CreateExpectationsMap(const std::string& filename);
    
    /**
     * @returns A vector of all filenames from the commandline.
     */
    std::vector<std::string>
    GetSourceFilenamesFromCommandline(const std::vector<std::string>& argv);
    
    
    //! Processing command line options
    void commandLineProcessing 
                              (int argc, char* argv[],
                               std::vector<std::string> & rose_cmdline);

    //! Set the project and all its files's midend error codes 
    void setMidendErrorCode (SgProject* project, int errorCode);

    //! Generates the file reports
    void generate_reports(SgProject* project,
                     std::vector< std::string> orig_rose_cmdline);

   // touch a file 
   void touch(const std::string& pathname);
  }// ::Rose::KeepGoing
}// ::Rose
#endif // ROSE_KEEPGOING_H

