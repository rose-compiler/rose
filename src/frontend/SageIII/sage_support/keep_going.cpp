/**
 * \file    signal_handling.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    November 16, 2013
 */
#include "sage3basic.h"

#include <iostream>

#include "keep_going.h"
#include "processSupport.h" // ROSE_ASSERT in ROSE/src/util

namespace Rose {
namespace KeepGoing {
bool g_keep_going = false;

#ifndef _MSC_VER
struct sigaction SignalAction;
#endif //_MSC_VER

bool set_signal_handler(SignalHandlerFunction handler)
{
#ifndef _MSC_VER
    SignalAction.sa_flags   = 0;
    SignalAction.sa_handler = handler;

    sigemptyset(&SignalAction.sa_mask);

    sigaction(SIGSEGV, &SignalAction, 0);
    sigaction(SIGABRT, &SignalAction, 0);
#else
    ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
#endif //_MSC_VER

    return true;
}

namespace Frontend {
  #ifndef _MSC_VER
    sigjmp_buf jmp_target;
  #endif //_MSC_VER
  void SignalHandler(int sig)
  {
      std::cout
          << "[WARN] "
          << "Caught signal="
          << "'" << sig << "' "
          << "during frontend processing"
          << std::endl;

      #ifndef _MSC_VER
          siglongjmp(Frontend::jmp_target, -1);
      #else
          ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
      #endif //_MSC_VER
  }

  namespace Commandline {
    #ifndef _MSC_VER
      sigjmp_buf jmp_target;
    #endif //_MSC_VER
    void SignalHandler(int sig)
    {
        std::cout
            << "[WARN] "
            << "Caught signal="
            << "'" << sig << "' "
            << "during commandline processing in frontend processing"
            << std::endl;

        #ifndef _MSC_VER
            siglongjmp(Frontend::Commandline::jmp_target, -1);
        #else
            ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
        #endif //_MSC_VER
    }
  }// ::Rose::KeepGoing::Frontend::Commandline

  namespace SecondaryPass {
    #ifndef _MSC_VER
      sigjmp_buf jmp_target;
    #endif //_MSC_VER
    void SignalHandler(int sig)
    {
        std::cout
            << "[WARN] "
            << "Caught signal="
            << "'" << sig << "' "
            << "during secondary pass in frontend processing"
            << std::endl;

        #ifndef _MSC_VER
            siglongjmp(Frontend::SecondaryPass::jmp_target, -1);
        #else
            ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
        #endif //_MSC_VER
    }
  }// ::Rose::KeepGoing::Frontend::SecondaryPass
}// ::Rose::KeepGoing::Frontend

namespace Midend {
  #ifndef _MSC_VER
    sigjmp_buf jmp_target;
  #endif //_MSC_VER
  void SignalHandler(int sig)
  {
      std::cout
          << "[WARN] "
          << "Caught signal="
          << "'" << sig << "' "
          << "during midend processing"
          << std::endl;

      #ifndef _MSC_VER
          siglongjmp(Midend::jmp_target, -1);
      #else
          ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
      #endif //_MSC_VER
  }
}// ::Rose::KeepGoing::Midend

namespace Backend {
namespace Unparser {
  #ifndef _MSC_VER
    sigjmp_buf jmp_target;
  #endif //_MSC_VER
  void SignalHandler(int sig)
  {
      std::cout
          << "[WARN] "
          << "Caught signal="
          << "'" << sig << "' "
          << "during backend unparser processing"
          << std::endl;

      #ifndef _MSC_VER
          siglongjmp(Backend::Unparser::jmp_target, -1);
      #else
          ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
      #endif //_MSC_VER
  }
}// ::Rose::KeepGoing::Backend::Unparser

namespace Compiler {
  #ifndef _MSC_VER
    sigjmp_buf jmp_target;
  #endif //_MSC_VER
  void SignalHandler(int sig)
  {
      std::cout
          << "[WARN] "
          << "Caught signal="
          << "'" << sig << "' "
          << "during backend compiler processing"
          << std::endl;

      #ifndef _MSC_VER
          siglongjmp(Backend::Compiler::jmp_target, -1);
      #else
          ROSE_ASSERT(! "[FATAL] KeepGoing feature not supported yet on Windows");
      #endif //_MSC_VER
  }
}// ::Rose::KeepGoing::Backend::Compiler
}// ::Rose::KeepGoing::Backend
}// ::Rose::KeepGoing
}// Rose

// TOO1 (05/14/2013): Handling for -rose:keep_going
//
// Compile the original source code file if:
//
// 1. Unparsing was skipped
// 2. The frontend encountered any errors, and the user specified to
//    "keep going" with -rose:keep_going.
//
//    Warning: Apparently, a frontend error code <= 3 indicates an EDG
//    frontend warning; however, existing logic says nothing about the
//    other language frontends' exit statuses.
bool
Rose::KeepGoing::Backend::UseOriginalInputFile(SgFile* file)
{
  ROSE_ASSERT(file != NULL);

#if 0
  // DQ (11/10/2016): Debugging information (detected that p_keep_going data member was uninitialized).
     printf ("file->get_unparse_output_filename().empty() = %s \n",file->get_unparse_output_filename().empty() ? "true" : "false");
     printf ("file->get_frontendErrorCode() != 0          = %s \n",(file->get_frontendErrorCode() != 0) ? "true" : "false");
     printf ("file->get_project()->get_midendErrorCode()  = %s \n",(file->get_project()->get_midendErrorCode()) ? "true" : "false");
     printf ("file->get_unparserErrorCode()               = %s \n",(file->get_unparserErrorCode()) ? "true" : "false");
     printf ("file->get_backendCompilerErrorCode() != 0   = %s \n",(file->get_backendCompilerErrorCode() != 0) ? "true" : "false");
     printf ("file->get_project()->get_keep_going()       = %s \n",file->get_project()->get_keep_going() ? "true" : "false");
#endif

  return
    // (1) An ROSE unparsed file was not generated
    file->get_unparse_output_filename().empty() == true ||
    // (2) File has an error and user specified to "keep going"
    (
        (
            file->get_frontendErrorCode() != 0 ||
            file->get_project()->get_midendErrorCode() != 0 ||
            file->get_unparserErrorCode() != 0 ||
            file->get_backendCompilerErrorCode() != 0) &&
        (
            file->get_project()->get_keep_going()
        )
    );
}// ::Rose::KeepGoing::Backend::UseOriginalInputFile

