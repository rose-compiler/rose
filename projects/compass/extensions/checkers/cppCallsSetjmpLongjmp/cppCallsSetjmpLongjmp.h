/*
 * Author: Gary M. Yuan
 * File: cppCallsSetjmpLongjmp.h
 * Date: 19-July-2007
 * Purpose: Cpp Calls Setjmp Longjmp header
 */

#include "compass.h"

#ifndef COMPASS_CPP_CALLS_SETJMP_LONGJMP_H
#define COMPASS_CPP_CALLS_SETJMP_LONGJMP_H

namespace CompassAnalyses
{ 
  namespace CppCallsSetjmpLongjmp
  { 
    /// \brief checkerName is a std::string containing the name of this checker.
    extern const std::string checkerName;
    /// \brief shortDescription is a std::string with a short description of
    /// this checker's pattern.
    extern const std::string shortDescription;
    /// \brief longDescription is a std::string with a detailed description
    /// of this checker's pattern and purpose.
    extern const std::string longDescription;

    // Specification of Checker Output Implementation
    ////////////////////////////////////////////////////////////////////////////
    /// The CheckerOutput class implements the violation output for this
    /// checker
    ////////////////////////////////////////////////////////////////////////////

    class CheckerOutput : public Compass::OutputViolationBase
    { 
      private:
        const char *what;
      public:
        std::string getString() const;
        CheckerOutput(SgNode* node, const char *w);
    }; //class CheckerOutput

    // Specification of Checker Traversal Implementation
    class Traversal : public AstSimpleProcessing, public Compass::TraversalBase
    {
      // Checker specific parameters should be allocated here.

      public:
        /// \brief The constructor
        Traversal(Compass::Parameters inputParameters, 
                  Compass::OutputObject* output);

        // The implementation of the run function has to match the 
        //traversal being called.
        /// \brief run, starts the AST traversal
        void run(SgNode* n){ this->traverse(n, preorder); };

        /// \brief visit, pattern for AST traversal
        void visit(SgNode* n);
    }; //class Traversal
  } //namespace CppCallsSetjmpLongjmp
} //namespace CompassAnalyses

// COMPASS_CPP_CALLS_SETJMP_LONGJMP_H
#endif 
