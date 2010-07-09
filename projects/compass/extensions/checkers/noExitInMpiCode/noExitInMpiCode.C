/*
 * Author: Gary M. Yuan
 * Date: 19-July-2007
 * File: noExitInMpiCode.h
 * Purpose: No Exit In Mpi Code header
 */

#include "rose.h"
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
    class Traversal : public Compass::AstSimpleProcessingWithRunFunction
    {
      // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

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

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

/*
 * Author: Gary M. Yuan
 * File: cppCallsSetjmpLongjmp.C
 * Date: 19-July-2007  * Purpose: Cpp Calls Setjmp Longjmp Analysis
 */ 

#include "compass.h"
// #include "noExitInMpiCode.h"

namespace CompassAnalyses
{ 
  namespace NoExitInMpiCode
  {
    const std::string checkerName      = "NoExitInMpiCode";

    // Descriptions should not include the newline character "\n".
    const std::string shortDescription = "finds call to exit() in MPI code block(s)";
    const std::string longDescription  = "Calling exit() from a parallel code will cause the code to deadlock. Even if every process calls exit collectively, this can leave some parallel environments in a hung state because MPI resources are not properly cleaned up.";
  } //namespace NoExitInMpiCode
} //namespace CompassAnalyses

CompassAnalyses::NoExitInMpiCode::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NoExitInMpiCode::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NoExitInMpiCode.YourParameter"]);


   }

/////////////////////////////////////////////////////////////////////////////////// The CompassAnalyses::NoExitInMpiCode::Traversal::visit(SgNode* node)
/// function implements a simple AST traversal seeking out SgFunctionRefExp
/// nodes for function reference expressions corresponding to MPI Init and
/// Finalize and calls to exit() nested in those blocks; these are reported
/// as checker violations.
///
/// \param node is a SgNode*
////////////////////////////////////////////////////////////////////////////////
void CompassAnalyses::NoExitInMpiCode::Traversal::visit(SgNode* node)
{
  static bool usedMPI = false;

  SgFunctionRefExp *sgfrexp = isSgFunctionRefExp(node);

  if( sgfrexp != NULL )
  {
    std::string sgfrexpName = sgfrexp->get_symbol()->get_name().getString();

    if( sgfrexpName == "MPI_Init" )
    {
      usedMPI = true;
    } //if( sgfrexpNam == "MPI_Init" )

    if( sgfrexpName == "MPI_Finalize" )
    {
      usedMPI = false;
    } //if( sgfrexpName == "MPI_Finalize" )

//#ASR:07/07/10
//fixed object creation of CheckerOutput for Cxx standard
    if( usedMPI == true && sgfrexpName == "exit" )
    {
      output->addOutput( new CompassAnalyses::NoExitInMpiCode::CheckerOutput( node ) );       
      //output->addOutput( new CompassAnalyses::NoExitInMpiCode::CheckerOutput::CheckerOutput( node ) );

    } //if( usedMPI == true && sgfrexpName == "exit" )

  } //if( sgfrexp != NULL )

  return;
} //visit(SgNode *node)

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NoExitInMpiCode::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NoExitInMpiCode::Traversal(params, output);
}

extern const Compass::Checker* const noExitInMpiCodeChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NoExitInMpiCode::checkerName,
        CompassAnalyses::NoExitInMpiCode::shortDescription,
        CompassAnalyses::NoExitInMpiCode::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
