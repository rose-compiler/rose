// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

/*
 * Author: Gary M. Yuan
 * File: cppCallsSetjmpLongjmp.C
 * Date: 19-July-2007  * Purpose: Cpp Calls Setjmp Longjmp Analysis
 */ 

#include "compass.h"
#include "noExitInMpiCode.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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

    if( usedMPI == true && sgfrexpName == "exit" )
    {
      output->addOutput( new CompassAnalyses::NoExitInMpiCode::CheckerOutput::CheckerOutput( node ) );
    } //if( usedMPI == true && sgfrexpName == "exit" )

  } //if( sgfrexp != NULL )

  return;
} //visit(SgNode *node)
