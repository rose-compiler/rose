/*
 * Author: Gary M. Yuan
 * File: cppCallsSetjmpLongjmp.h
 * Date: 19-July-2007
 * Purpose: Cpp Calls Setjmp Longjmp header
 */

#include "rose.h"
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
        /// \brief run, starts the AST traversal
        void run(SgNode* n){ this->traverse(n, preorder); };

        /// \brief visit, pattern for AST traversal
        void visit(SgNode* n);
    }; //class Traversal
  } //namespace CppCallsSetjmpLongjmp
} //namespace CompassAnalyses

// COMPASS_CPP_CALLS_SETJMP_LONGJMP_H
#endif 
// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

/*
 * Author: Gary M. Yuan
 * File: cppCallsSetjmpLongjmp.C 
 * Date: 19-July-2007
 * Purpose: Cpp Calls Setjmp Longjmp Analysis
 */

#include "compass.h"
// #include "cppCallsSetjmpLongjmp.h"

namespace CompassAnalyses
{ 
  namespace CppCallsSetjmpLongjmp
  { 
    const std::string checkerName      = "CppCallsSetjmpLongjmp";

    // Descriptions should not include the newline character "\n".
    const std::string shortDescription = "finds calls to setjmp() or longjmp() in C++ code";
    const std::string longDescription  = "Do not use setjmp() or longjmp() in C++ code. These functions provide exception handling for C programs. These cannot be safely used in C++ code because the exception handling mechanism they implement does not adhere to normal object lifecycle semantics--a jump will not results in destruction of scoped, automatically allocated objects";
  } //End of namespace CppCallsSetjmpLongjmp.
} //End of namespace CompassAnalyses.

std::string 
CompassAnalyses::CppCallsSetjmpLongjmp::CheckerOutput::getString() const
{
  ROSE_ASSERT(getNodeArray().size() <= 1);

  // Default implementation for getString
  SgLocatedNode* locatedNode = isSgLocatedNode(getNode());
  std::string sourceCodeLocation;
  if(locatedNode != NULL)
  {
    Sg_File_Info* start = locatedNode->get_startOfConstruct();
    Sg_File_Info* end   = locatedNode->get_endOfConstruct();
    sourceCodeLocation = (end ? 
    Compass::formatStandardSourcePosition(start, end) : 
    Compass::formatStandardSourcePosition(start));        
  } //if(locatedNode != NULL)
  else
  {
    // Else this could be a SgInitializedName or SgTemplateArgument 
    //(not yet moved to be a SgLocatedNode)
    Sg_File_Info* start = getNode()->get_file_info();
    ROSE_ASSERT(start != NULL);
    sourceCodeLocation = Compass::formatStandardSourcePosition(start);
  } //else

  std::string nodeName = getNode()->class_name();
  return m_checkerName + ": " + sourceCodeLocation + ": " + nodeName + ": " 
         + m_shortDescription + " calls: " + what;
} //CompassAnalyses::CppCallsSetjmpLongjmp::CheckerOutput::getString()

CompassAnalyses::
CppCallsSetjmpLongjmp::CheckerOutput::CheckerOutput( SgNode* node, 
   const char *w )
   : OutputViolationBase(node,checkerName,shortDescription), what(w)
{
} //CompassAnalyses::CppCallsSetjmpLongjmp::CheckerOutput::CheckerOutput()

CompassAnalyses::CppCallsSetjmpLongjmp::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["CppCallsSetjmpLongjmp.YourParameter"]);
}

void CompassAnalyses::CppCallsSetjmpLongjmp::Traversal::visit(SgNode* node)
{ 
  SgFunctionRefExp *sgfrexp = isSgFunctionRefExp(node);

  if( sgfrexp != NULL )
  {
    if( node->get_file_info()->get_filenameString().find( ".c" ) != 
        std::string::npos )
    {
      return;
    } //if

    std::string sgfrexpName = sgfrexp->get_symbol()->get_name().getString();

//#ASR: 07/07/10
//fixed the object creation of CheckerOutput for Cxx standard    
    if( sgfrexpName == "_setjmp" )
    {
      output->addOutput( new CompassAnalyses::CppCallsSetjmpLongjmp::CheckerOutput( node, "setjmp()" ) );
//      output->addOutput( new CompassAnalyses::CppCallsSetjmpLongjmp::CheckerOutput::CheckerOutput( node, "setjmp()" ) );
    } //if( sgfrexpName == "_setjmp" )
    else if( sgfrexpName == "longjmp" )
    {
      output->addOutput( new CompassAnalyses::CppCallsSetjmpLongjmp::CheckerOutput( node, "longjmp()" ) );
//      output->addOutput( new CompassAnalyses::CppCallsSetjmpLongjmp::CheckerOutput::CheckerOutput( node, "longjmp()" ) );     
    } //else if( sgfrexpName == "longjmp" )
  } //if( sgfrexp != NULL )

  return;
} //CompassAnalyses::CppCallsSetjmpLongjmp::Traversal::visit()
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::CppCallsSetjmpLongjmp::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::CppCallsSetjmpLongjmp::Traversal(params, output);
}

extern const Compass::Checker* const cppCallsSetjmpLongjmpChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::CppCallsSetjmpLongjmp::checkerName,
        CompassAnalyses::CppCallsSetjmpLongjmp::shortDescription,
        CompassAnalyses::CppCallsSetjmpLongjmp::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
