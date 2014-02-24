#ifndef ROSE_KEEPGOING_H
#define ROSE_KEEPGOING_H

/**
 * \file    signal_handling.h
 * \author  Justin Too <too1@llnl.gov>
 * \date    November 16, 2013
 */

#include <setjmp.h>
#include <signal.h>

namespace ROSE {
namespace KeepGoing {
//-----------------------------------------------------------------------------
// Global Data
//-----------------------------------------------------------------------------
extern bool g_keep_going;
extern struct sigaction SignalAction;
typedef void(*SignalHandlerFunction)(int);


//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------
#ifndef _MSC_VER
    #define KEEP_GOING_CAUGHT_COMMANDLINE_SIGNAL                                \
      (                                                                         \
          ROSE::KeepGoing::g_keep_going &&                                      \
          ROSE::KeepGoing::set_signal_handler(                                  \
              &ROSE::KeepGoing::Frontend::Commandline::SignalHandler) &&        \
          sigsetjmp(ROSE::KeepGoing::Frontend::Commandline::jmp_target, 0) != 0 \
      )
    #define KEEP_GOING_CAUGHT_FRONTEND_SIGNAL                         \
      (                                                               \
          ROSE::KeepGoing::g_keep_going &&                            \
          ROSE::KeepGoing::set_signal_handler(                        \
              &ROSE::KeepGoing::Frontend::SignalHandler) &&           \
          sigsetjmp(ROSE::KeepGoing::Frontend::jmp_target, 0) != 0    \
      )
    #define KEEP_GOING_CAUGHT_FRONTEND_SECONDARY_PASS_SIGNAL                        \
      (                                                                             \
          ROSE::KeepGoing::g_keep_going &&                                          \
          ROSE::KeepGoing::set_signal_handler(                                      \
              &ROSE::KeepGoing::Frontend::SecondaryPass::SignalHandler) &&          \
          sigsetjmp(ROSE::KeepGoing::Frontend::SecondaryPass::jmp_target, 0) != 0   \
      )
    #define KEEP_GOING_CAUGHT_MIDEND_SIGNAL                         \
      (                                                             \
          ROSE::KeepGoing::g_keep_going &&                          \
          ROSE::KeepGoing::set_signal_handler(                      \
              &ROSE::KeepGoing::Midend::SignalHandler) &&           \
          sigsetjmp(ROSE::KeepGoing::Midend::jmp_target, 0) != 0    \
      )
    #define KEEP_GOING_CAUGHT_BACKEND_UNPARSER_SIGNAL                       \
      (                                                                     \
          ROSE::KeepGoing::g_keep_going &&                                  \
          ROSE::KeepGoing::set_signal_handler(                              \
              &ROSE::KeepGoing::Backend::Unparser::SignalHandler) &&        \
          sigsetjmp(ROSE::KeepGoing::Backend::Unparser::jmp_target, 0) != 0 \
      )
    #define KEEP_GOING_CAUGHT_BACKEND_COMPILER_SIGNAL                       \
      (                                                                     \
          ROSE::KeepGoing::g_keep_going &&                                  \
          ROSE::KeepGoing::set_signal_handler(                              \
              &ROSE::KeepGoing::Backend::Compiler::SignalHandler) &&        \
          sigsetjmp(ROSE::KeepGoing::Backend::Compiler::jmp_target, 0) != 0 \
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
  }// ::ROSE::KeepGoing::Frontend::Commandline

  namespace SecondaryPass {
    #ifndef _MSC_VER
      extern sigjmp_buf jmp_target;
    #endif //_MSC_VER
    void SignalHandler(int sig);
  }// ::ROSE::KeepGoing::Frontend::SecondaryPass
}// ::ROSE::KeepGoing::Frontend

namespace Midend {
  #ifndef _MSC_VER
    extern sigjmp_buf jmp_target;
  #endif //_MSC_VER
    void SignalHandler(int sig);
}// ::ROSE::KeepGoing::Midend

namespace Backend {
namespace Unparser {
  #ifndef _MSC_VER
    extern sigjmp_buf jmp_target;
  #endif //_MSC_VER
  void SignalHandler(int sig);
}// ::ROSE::KeepGoing::Backend::Unparser

namespace Compiler {
  #ifndef _MSC_VER
    extern sigjmp_buf jmp_target;
  #endif //_MSC_VER
  void SignalHandler(int sig);
}// ::ROSE::KeepGoing::Backend::Compiler

}// ::ROSE::KeepGoing::Backend
}// ::ROSE::KeepGoing
}// ::ROSE
#endif // ROSE_KEEPGOING_H

