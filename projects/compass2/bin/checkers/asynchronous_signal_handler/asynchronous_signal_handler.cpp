/**
 * \file asynchronous_signal_handler.cpp
 * \author Sam Kelly <kelly64@llnl.gov, kellys@dickinson.edu>
 * \date Monday, July 22, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const asynchronousSignalHandlerChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_ASYNCHRONOUS_SIGNAL_HANDLER_H
#define COMPASS_ASYNCHRONOUS_SIGNAL_HANDLER_H

namespace CompassAnalyses
{
/**
 * \brief Makes sure that signal handlers only use async safe STL functions
 */
namespace AsynchronousSignalHandler
{
extern const string checker_name;
extern const string short_description;
extern const string long_description;
extern       string source_directory;

/**
 * \brief Specificaiton of checker results.
 */
class CheckerOutput: public Compass::OutputViolationBase {
 public:
  explicit CheckerOutput(SgNode *const node);

 private:
  DISALLOW_COPY_AND_ASSIGN(CheckerOutput);
};

bool IsNodeNotInUserLocation(const SgNode* node)
{
  const SgLocatedNode* located_node = isSgLocatedNode(node);
  if (located_node != NULL)
  {
    return ! Compass::IsNodeInUserLocation(
        located_node,
        AsynchronousSignalHandler::source_directory);
  }
  else
  {
    return true;
  }
};

} // ::CompassAnalyses
} // ::AsynchronousSignalHandler
#endif // COMPASS_ASYNCHRONOUS_SIGNAL_HANDLER_H

/*-----------------------------------------------------------------------------
 * Implementation
 *
 * 1. Maintain a Hash Set of all async safe functions (ensuring that this
 *    only has to be generated once.
 *
 * 2. For each SgFunctionDefinition, scan for SgFunctionCallExp's.
 *
 * 3. If it is a signal call, then set marked = true
 *
 * 4. If it is not in the async_safe hash_set, add it to the list of
 *    violating function calls found in the current parent function
 *
 * 6. Once all function calls have been examined, if marked was set to
 *    true, report output for each violating function that was found
 *    during the search
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
namespace AsynchronousSignalHandler
{
const string checker_name      = "AsynchronousSignalHandler";
const string short_description = "non async safe function found in signal handler";
const string long_description  = "Single handlers must not use functions that are not async-safe, else the results are undefined acording to the C standard";
string source_directory = "/";
}
}

CompassAnalyses::AsynchronousSignalHandler::
CheckerOutput::CheckerOutput(SgNode *const node)
: OutputViolationBase(node,
                      ::asynchronousSignalHandlerChecker->checkerName,
                       ::asynchronousSignalHandlerChecker->shortDescription) {}

boost::unordered_set<std::string> async_safe;
bool first_run = true;

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
{
  // We only care about source code in the user's space, not,
  // for example, Boost or system files.
  string target_directory =
      parameters["general::target_directory"].front();
  CompassAnalyses::AsynchronousSignalHandler::source_directory.assign(target_directory);

  // Use the pre-built ROSE AST
  SgProject* sageProject = Compass::projectPrerequisite.getProject();
  SgNode *root_node = (SgNode *)sageProject;

  if(first_run)
  {
    first_run = false;
    async_safe.insert("_Exit");
    async_safe.insert("_exit");
    async_safe.insert("abort");
    async_safe.insert("accept");
    async_safe.insert("access");
    async_safe.insert("aio_error");
    async_safe.insert("aio_return");
    async_safe.insert("aio_suspend");
    async_safe.insert("alarm");
    async_safe.insert("bind");
    async_safe.insert("cfgetispeed");
    async_safe.insert("cfgetospeed");
    async_safe.insert("cfsetispeed");
    async_safe.insert("cfsetospeed");
    async_safe.insert("chdir");
    async_safe.insert("chmod");
    async_safe.insert("chown");
    async_safe.insert("clock_gettime");
    async_safe.insert("close");
    async_safe.insert("connect");
    async_safe.insert("creat");
    async_safe.insert("dup");
    async_safe.insert("dup2");
    async_safe.insert("execle");
    async_safe.insert("execve");
    async_safe.insert("fchmod");
    async_safe.insert("fchown");
    async_safe.insert("fcntl");
    async_safe.insert("fdatasync");
    async_safe.insert("fork");
    async_safe.insert("fpathconf");
    async_safe.insert("fstat");
    async_safe.insert("fsync");
    async_safe.insert("ftruncate");
    async_safe.insert("getegid");
    async_safe.insert("geteuid");
    async_safe.insert("getgid");
    async_safe.insert("getgroups");
    async_safe.insert("getpeername");
    async_safe.insert("getpgrp");
    async_safe.insert("getpid");
    async_safe.insert("getppid");
    async_safe.insert("getsockname");
    async_safe.insert("getsockopt");
    async_safe.insert("getuid");
    async_safe.insert("kill");
    async_safe.insert("link");
    async_safe.insert("listen");
    async_safe.insert("lseek");
    async_safe.insert("lstat");
    async_safe.insert("mkdir");
    async_safe.insert("mkfifo");
    async_safe.insert("open");
    async_safe.insert("pathconf");
    async_safe.insert("pause");
    async_safe.insert("pipe");
    async_safe.insert("poll");
    async_safe.insert("posix_trace_event");
    async_safe.insert("pselect");
    async_safe.insert("raise");
    async_safe.insert("read");
    async_safe.insert("readlink");
    async_safe.insert("recv");
    async_safe.insert("recvfrom");
    async_safe.insert("recvmsg");
    async_safe.insert("rename");
    async_safe.insert("rmdir");
    async_safe.insert("select");
    async_safe.insert("sem_post");
    async_safe.insert("send");
    async_safe.insert("sendmsg");
    async_safe.insert("sendto");
    async_safe.insert("setgid");
    async_safe.insert("setpgid");
    async_safe.insert("setsid");
    async_safe.insert("setsockopt");
    async_safe.insert("setuid");
    async_safe.insert("shutdown");
    async_safe.insert("sigaction");
    async_safe.insert("sigaddset");
    async_safe.insert("sigdelset");
    async_safe.insert("sigemptyset");
    async_safe.insert("sigfillset");
    async_safe.insert("sigismember");
    async_safe.insert("sleep");
    async_safe.insert("signal");
    async_safe.insert("sigpause");
    async_safe.insert("sigpending");
    async_safe.insert("sigprocmask");
    async_safe.insert("sigqueue");
    async_safe.insert("sigset");
    async_safe.insert("sigsuspend");
    async_safe.insert("sockatmark");
    async_safe.insert("socket");
    async_safe.insert("socketpair");
    async_safe.insert("stat");
    async_safe.insert("symlink");
    async_safe.insert("sysconf");
    async_safe.insert("tcdrain");
    async_safe.insert("tcflow");
    async_safe.insert("tcflush");
    async_safe.insert("tcgetattr");
    async_safe.insert("tcgetpgrp");
    async_safe.insert("tcsendbreak");
    async_safe.insert("tcsetattr");
    async_safe.insert("tcsetpgrp");
    async_safe.insert("time");
    async_safe.insert("timer_getoverrun");
    async_safe.insert("timer_gettime");
    async_safe.insert("timer_settime");
    async_safe.insert("times");
    async_safe.insert("umask");
    async_safe.insert("uname");
    async_safe.insert("unlink");
    async_safe.insert("utime");
    async_safe.insert("wait");
    async_safe.insert("waitpid");
    async_safe.insert("write");
  }


  AstMatching func_def_matcher;
  MatchResult func_def_matches = func_def_matcher
      .performMatching("$f=SgFunctionDefinition", root_node);
  BOOST_FOREACH(SingleMatchVarBindings func_def_match, func_def_matches)
  {

    SgFunctionDefinition *parent_func_def =
        (SgFunctionDefinition *)func_def_match["$f"];

    AstMatching func_call_matcher;
    MatchResult func_call_matches = func_call_matcher
        .performMatching("$f=SgFunctionCallExp", parent_func_def);
    bool marked = false;
    std::vector<SgFunctionCallExp*> failed_calls;
    BOOST_FOREACH(SingleMatchVarBindings func_call_match, func_call_matches)
    {
      SgFunctionCallExp *func_call = (SgFunctionCallExp *)func_call_match["$f"];
      SgFunctionRefExp *func_ref = (SgFunctionRefExp *)func_call->get_traversalSuccessorByIndex(0);
      if(func_ref == NULL) continue;
      SgFunctionDeclaration *func_dec = func_call->getAssociatedFunctionDeclaration();
      if(func_dec == NULL) continue;
      SgExprListExp *func_args = func_call->get_args();
      if(func_args == NULL) continue;

      if((func_args->get_expressions().size() == 2) &&
          (func_dec->get_definingDeclaration() == NULL) &&
          (func_dec->get_name() == "signal"))
      {
        // the parent function contains a signal() call
        marked = true;
      }

      if(async_safe.find(func_dec->get_name().getString()) == async_safe.end())
      {
        // this is not an async safe function!
        failed_calls.push_back(func_call);
      }
    }
    if(marked)
    {
      BOOST_FOREACH(SgFunctionCallExp *func_call, failed_calls)
      {
        output->addOutput(
            new CompassAnalyses::AsynchronousSignalHandler::
            CheckerOutput(func_call));
      }
    }
  }
}




// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
{
  return NULL;
}

extern const Compass::Checker* const asynchronousSignalHandlerChecker =
    new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::AsynchronousSignalHandler::checker_name,
        // Descriptions should not include the newline character "\n".
        CompassAnalyses::AsynchronousSignalHandler::short_description,
        CompassAnalyses::AsynchronousSignalHandler::long_description,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);

