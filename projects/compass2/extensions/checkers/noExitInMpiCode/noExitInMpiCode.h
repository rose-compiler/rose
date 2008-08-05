/*
 * Author: Gary M. Yuan
 * Date: 19-July-2007
 * File: noExitInMpiCode.h
 * Purpose: No Exit In Mpi Code header
 */

#include "compass.h"

#ifndef COMPASS_NO_EXIT_IN_MPI_CODE_H
#define COMPASS_NO_EXIT_IN_MPI_CODE_H

namespace CompassAnalyses
{ 
  namespace NoExitInMpiCode
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
    /// checker.
    ////////////////////////////////////////////////////////////////////////////

    class CheckerOutput : public Compass::OutputViolationBase
    { 
      public:
        CheckerOutput(SgNode* node);
    }; //class CheckerOutput

    // Specification of Checker Traversal Implementation
    ////////////////////////////////////////////////////////////////////////////
    /// The Traversal class implements a simple AST traversal for this checker
    /// seeking out function reference expressions corresponding to exit()
    /// between blocks of MPI code.
    ////////////////////////////////////////////////////////////////////////////
    class Traversal : public AstSimpleProcessing, public Compass::TraversalBase
    {
      // Checker specific parameters should be allocated here.

      public:
        /// \brief The constructor
        Traversal(Compass::Parameters inputParameters, 
                    Compass::OutputObject* output);

        // The implementation of the run function has to match the 
        //traversal being called.
         /// \brief run, starts AST traversal
         void run(SgNode* n){ this->traverse(n, preorder); };
         /// \brief visit, pattern for AST traversal 
         void visit(SgNode* n);

    }; //class Traversal
  } //namespace NoExitInMpiCode
} //namespace CompassAnalyses

// COMPASS_NO_EXIT_IN_MPI_CODE_H
#endif 

