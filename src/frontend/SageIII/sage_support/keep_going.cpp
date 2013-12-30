/**
 * \file    signal_handling.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    November 16, 2013
 */

#include <iostream>

#include "keep_going.h"

namespace ROSE {
namespace KeepGoing {
bool g_keep_going = false;
struct sigaction SignalAction;

bool set_signal_handler(SignalHandlerFunction handler)
{
    SignalAction.sa_flags   = 0;
    SignalAction.sa_handler = handler;

    sigemptyset(&SignalAction.sa_mask);

    sigaction(SIGSEGV, &SignalAction, 0);
    sigaction(SIGABRT, &SignalAction, 0);

    return true;
}

namespace Frontend {
  sigjmp_buf jmp_target;
  void SignalHandler(int sig)
  {
      std::cout
          << "[WARN] "
          << "Caught signal="
          << "'" << sig << "' "
          << "during frontend processing"
          << std::endl;

      siglongjmp(Frontend::jmp_target, -1);
  }

  namespace SecondaryPass {
    sigjmp_buf jmp_target;
    void SignalHandler(int sig)
    {
        std::cout
            << "[WARN] "
            << "Caught signal="
            << "'" << sig << "' "
            << "during secondary pass in frontend processing"
            << std::endl;

        siglongjmp(Frontend::SecondaryPass::jmp_target, -1);
    }
  }// ::ROSE::KeepGoing::Frontend::SecondaryPass
}// ::ROSE::KeepGoing::Frontend

namespace Backend {
namespace Unparser {
  sigjmp_buf jmp_target;
  void SignalHandler(int sig)
  {
      std::cout
          << "[WARN] "
          << "Caught signal="
          << "'" << sig << "' "
          << "during backend unparser processing"
          << std::endl;

      siglongjmp(Backend::Unparser::jmp_target, -1);
  }
}// ::ROSE::KeepGoing::Backend::Unparser
}// ::ROSE::KeepGoing::Backend
}// ::ROSE::KeepGoing
}// ::ROSE

