/**
 * \file    signal_handling.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    November 16, 2013
 */
#include <iostream>

#include "keep_going.h"
#include "processSupport.h" // ROSE_ASSERT in ROSE/src/util

namespace ROSE {
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
  }// ::ROSE::KeepGoing::Frontend::Commandline

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
  }// ::ROSE::KeepGoing::Frontend::SecondaryPass
}// ::ROSE::KeepGoing::Frontend

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
}// ::ROSE::KeepGoing::Backend::Unparser

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
}// ::ROSE::KeepGoing::Backend::Compiler
}// ::ROSE::KeepGoing::Backend
}// ::ROSE::KeepGoing
}// ::ROSE

