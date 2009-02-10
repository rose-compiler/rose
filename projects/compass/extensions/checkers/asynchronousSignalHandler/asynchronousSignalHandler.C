// Asynchronous Signal Handler
// Author: Daniel J. Quinlan
// Date: 05-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_ASYNCHRONOUS_SIGNAL_HANDLER_H
#define COMPASS_ASYNCHRONOUS_SIGNAL_HANDLER_H

namespace CompassAnalyses
   { 
     namespace AsynchronousSignalHandler
        { 
        /*! \brief Asynchronous Signal Handler: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_ASYNCHRONOUS_SIGNAL_HANDLER_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Asynchronous Signal Handler Analysis
// Author: Daniel J. Quinlan
// Date: 05-August-2007

#include "compass.h"
// #include "asynchronousSignalHandler.h"

namespace CompassAnalyses
   { 
     namespace AsynchronousSignalHandler
        { 
          const std::string checkerName      = "AsynchronousSignalHandler";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "non async safe function found in signal handler";
          const std::string longDescription  = "Single handlers must not use functions that are not async-safe, else the results are undefined acording to the C standard";

       // Added STL set and function to initialize the set of acceptable async-safe functions.
          std::set<std::string> asyncSignalSafeFunctionList;
          void setupAsyncSignalSafeFunctionList();
          std::set<SgFunctionDeclaration*> listOfProcessedHandlers;

       // Traversal to be used on each handler function to search for non-async-safe functions
          class HandlerTraversal : public Compass::AstSimpleProcessingWithRunFunction
             {
               Compass::OutputObject* output;
               public:
                 HandlerTraversal(Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };

        } //End of namespace AsynchronousSignalHandler.
   } //End of namespace CompassAnalyses.

CompassAnalyses::AsynchronousSignalHandler::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::AsynchronousSignalHandler::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["AsynchronousSignalHandler.YourParameter"]);


   }

// Constructor for traversal over handler declaration
CompassAnalyses::AsynchronousSignalHandler::HandlerTraversal::
HandlerTraversal(Compass::OutputObject* output)
   : output(output)
   {
   }

void
CompassAnalyses::AsynchronousSignalHandler::HandlerTraversal::
visit(SgNode* node)
   {
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(node);
     if (functionCallExp != NULL)
        {
          SgExpression* function = functionCallExp->get_function();
          SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(function);
          if (functionRefExp != NULL)
             {
               SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
               ROSE_ASSERT(functionSymbol != NULL);

            // Get the function declaration from the function symbol
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionSymbol->get_declaration());
               ROSE_ASSERT (functionDeclaration != NULL);

              std::string functionName = functionDeclaration->get_name().getString();
              std::set<std::string>::iterator i = asyncSignalSafeFunctionList.find(functionName);
              if (i != asyncSignalSafeFunctionList.end())
                 {
                // Found an async-safe function call in a signal handler
                 }
                else
                 {
                // This is a function call to a function NOT on the list of async-safe functions
                   output->addOutput(new CheckerOutput(functionCallExp));
                 }
             }
        }

  // This case was not in the original version sent to CERT.
     SgExprStatement* expressionStatement = isSgExprStatement(node);
     if (expressionStatement != NULL)
        {
       // This is where we still have to look for L-values that are non-local and not of 
       // type: sig_atomic_t. And such L-values detects should cause the output of a violation.
        }
   }

void
CompassAnalyses::AsynchronousSignalHandler::Traversal::
visit(SgNode* node)
   {
  // Check for the top of the AST and setup the list of functions (else make it a static list).
     if (isSgProject(node) != NULL)
        {
       // Empty the accumulated list of previously processed signal handlers
          listOfProcessedHandlers.erase(listOfProcessedHandlers.begin(),listOfProcessedHandlers.end());
          setupAsyncSignalSafeFunctionList();
        }

     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(node);
     if (functionCallExp != NULL)
        {
          SgExpression* function = functionCallExp->get_function();
          SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(function);
          if (functionRefExp != NULL)
             {
               SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
               ROSE_ASSERT(functionSymbol != NULL);

            // Get the function declaration from the function symbol
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionSymbol->get_declaration());
               ROSE_ASSERT (functionDeclaration != NULL);

            // Get the function arguments
               SgExprListExp* functionArguments = functionCallExp->get_args();
               ROSE_ASSERT(functionArguments != NULL);

            // Additional (more strict) tests could be added here to make sure this is non an overloaded function.
            // Here I have verified that this is a two parameter function not defined in the source code and which 
            // is named "signal".  The function type (SgFunctionType) and the scope of the declaration could be 
            // evaluated for more precision.
               bool foundSignalFunctionCall = (functionArguments->get_expressions().size() == 2) && 
                                              (functionDeclaration->get_definingDeclaration() == NULL) && 
                                              (functionDeclaration->get_name() == "signal");

               if (foundSignalFunctionCall == true)
                  {
                    SgExpressionPtrList::iterator argIterator = functionArguments->get_expressions().begin();
                    argIterator++;
                 // We already verified that there were 2 arguments, so this is safe
                    SgExpression* secondFunctionParameter = *argIterator;
                    ROSE_ASSERT(secondFunctionParameter != NULL);

                 // If the handler function was provide as a result of a function call (e.g. "signal(n,getHandler());" 
                 // then this code would have to be more general.
                    SgFunctionRefExp* signalHandlerFunctionReference = isSgFunctionRefExp(secondFunctionParameter);
                    ROSE_ASSERT(signalHandlerFunctionReference != NULL);
                    SgFunctionSymbol* signalHandlerFunctionSymbol = signalHandlerFunctionReference->get_symbol();
                    ROSE_ASSERT(signalHandlerFunctionSymbol != NULL);

                 // Get the handler's function declaration from the SgFunctionSymbol
                 // Only use the defining declaration, to avoid and references to function prototype declarations.
                    ROSE_ASSERT(signalHandlerFunctionSymbol->get_declaration() != NULL);
                    SgFunctionDeclaration* signalHandlerFunctionDeclaration = isSgFunctionDeclaration(signalHandlerFunctionSymbol->get_declaration()->get_definingDeclaration());
                    ROSE_ASSERT (signalHandlerFunctionDeclaration != NULL);

                 // Check if this signalHandlerFunctionDeclaration has been processed previously.
                    if (listOfProcessedHandlers.find(signalHandlerFunctionDeclaration) == listOfProcessedHandlers.end())
                       {
                      // Now traverse the handler implementation (function definition) to detect non async-safe functions
                         HandlerTraversal nestedTraversal(output);
                         nestedTraversal.run(signalHandlerFunctionDeclaration);

                      // Since we only want to traverse the handlers once and there could be many calls to "signal" that 
                      // reference them, we keep a set of the handlers that have been checked.
                         listOfProcessedHandlers.insert(signalHandlerFunctionDeclaration);
                       }
                  }
             }
        }
   }


void
CompassAnalyses::AsynchronousSignalHandler::setupAsyncSignalSafeFunctionList()
   {
  // This function sets up the list of function names that are acceptable within signal handlers.

     std::set<std::string> & s = asyncSignalSafeFunctionList;

     s.insert("_Exit");
     s.insert("_exit");
     s.insert("abort");
     s.insert("accept");
     s.insert("access");
     s.insert("aio_error");
     s.insert("aio_return");
     s.insert("aio_suspend");
     s.insert("alarm");
     s.insert("bind");
     s.insert("cfgetispeed");
     s.insert("cfgetospeed");
     s.insert("cfsetispeed");
     s.insert("cfsetospeed");
     s.insert("chdir");
     s.insert("chmod");
     s.insert("chown");
     s.insert("clock_gettime");
     s.insert("close");
     s.insert("connect");
     s.insert("creat");
     s.insert("dup");
     s.insert("dup2");
     s.insert("execle");
     s.insert("execve");
     s.insert("fchmod");
     s.insert("fchown");
     s.insert("fcntl");
     s.insert("fdatasync");
     s.insert("fork");
     s.insert("fpathconf");
     s.insert("fstat");
     s.insert("fsync");
     s.insert("ftruncate");
     s.insert("getegid");
     s.insert("geteuid");
     s.insert("getgid");
     s.insert("getgroups");
     s.insert("getpeername");
     s.insert("getpgrp");
     s.insert("getpid");
     s.insert("getppid");
     s.insert("getsockname");
     s.insert("getsockopt");
     s.insert("getuid");
     s.insert("kill");
     s.insert("link");
     s.insert("listen");
     s.insert("lseek");
     s.insert("lstat");
     s.insert("mkdir");
     s.insert("mkfifo");
     s.insert("open");
     s.insert("pathconf");
     s.insert("pause");
     s.insert("pipe");
     s.insert("poll");
     s.insert("posix_trace_event");
     s.insert("pselect");
     s.insert("raise");
     s.insert("read");
     s.insert("readlink");
     s.insert("recv");
     s.insert("recvfrom");
     s.insert("recvmsg");
     s.insert("rename");
     s.insert("rmdir");
     s.insert("select");
     s.insert("sem_post");
     s.insert("send");
     s.insert("sendmsg");
     s.insert("sendto");
     s.insert("setgid");
     s.insert("setpgid");
     s.insert("setsid");
     s.insert("setsockopt");
     s.insert("setuid");
     s.insert("shutdown");
     s.insert("sigaction");
     s.insert("sigaddset");
     s.insert("sigdelset");
     s.insert("sigemptyset");
     s.insert("sigfillset");
     s.insert("sigismember");
     s.insert("sleep");
     s.insert("signal");
     s.insert("sigpause");
     s.insert("sigpending");
     s.insert("sigprocmask");
     s.insert("sigqueue");
     s.insert("sigset");
     s.insert("sigsuspend");
     s.insert("sockatmark");
     s.insert("socket");
     s.insert("socketpair");
     s.insert("stat");
     s.insert("symlink");
     s.insert("sysconf");
     s.insert("tcdrain");
     s.insert("tcflow");
     s.insert("tcflush");
     s.insert("tcgetattr");
     s.insert("tcgetpgrp");
     s.insert("tcsendbreak");
     s.insert("tcsetattr");
     s.insert("tcsetpgrp");
     s.insert("time");
     s.insert("timer_getoverrun");
     s.insert("timer_gettime");
     s.insert("timer_settime");
     s.insert("times");
     s.insert("umask");
     s.insert("uname");
     s.insert("unlink");
     s.insert("utime");
     s.insert("wait");
     s.insert("waitpid");
     s.insert("write");
   }


static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::AsynchronousSignalHandler::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::AsynchronousSignalHandler::Traversal(params, output);
}

extern const Compass::Checker* const asynchronousSignalHandlerChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::AsynchronousSignalHandler::checkerName,
        CompassAnalyses::AsynchronousSignalHandler::shortDescription,
        CompassAnalyses::AsynchronousSignalHandler::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
