// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Function Definition Prototype Analysis
// Author: Gary M. Yuan
// Date: 23-July-2007

#include "compass.h"
#include "functionDefinitionPrototype.h"

namespace CompassAnalyses
   { 
     namespace FunctionDefinitionPrototype
        { 
          const std::string checkerName      = "FunctionDefinitionPrototype";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "matching function prototype not available";
          const std::string longDescription  = "This checker checks that function definitions have matching function prototypes";
        } //End of namespace FunctionDefinitionPrototype.
   } //End of namespace CompassAnalyses.

CompassAnalyses::FunctionDefinitionPrototype::
CheckerOutput::CheckerOutput ( SgNode* node, const char *w )
   : OutputViolationBase(node,checkerName,shortDescription), what(w)
   {
   }

#if 0
// This function is not requied since the base class function is available and is preferred (for conformity).
std::string
CompassAnalyses::FunctionDefinitionPrototype::CheckerOutput::getString() const
{
     ROSE_ASSERT(getNodeArray().size() <= 1);

  // Default implementation for getString
     SgLocatedNode* locatedNode = isSgLocatedNode(getNode());
     std::string sourceCodeLocation;
     if (locatedNode != NULL)
        {
          Sg_File_Info* start = locatedNode->get_startOfConstruct();
          Sg_File_Info* end   = locatedNode->get_endOfConstruct();
          sourceCodeLocation = (end ? Compass::formatStandardSourcePosition(start, end) 
                                    : Compass::formatStandardSourcePosition(start));
       }
      else
       {
      // Else this could be a SgInitializedName or SgTemplateArgument (not yet moved to be a SgLocatedNode)
         Sg_File_Info* start = getNode()->get_file_info();
         ROSE_ASSERT(start != NULL);
         sourceCodeLocation = Compass::formatStandardSourcePosition(start);
       }

     std::string nodeName = getNode()->class_name();

  // The short description used here needs to be put into a separate function (can this be part of what is filled in by the script?)
  // return loc + ": " + nodeName + ": variable requiring static constructor initialization";

     return m_checkerName + ": " + sourceCodeLocation + ": " + nodeName + ": " + m_shortDescription + ".\n\"" + what + "\" does not have a prototype";
} //CompassAnalyses::FunctionDefinitionPrototype::CheckerOutput::getString()
#endif

CompassAnalyses::FunctionDefinitionPrototype::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FunctionDefinitionPrototype.YourParameter"]);


   }

void
CompassAnalyses::FunctionDefinitionPrototype::Traversal::
visit(SgNode* node)
   { 
     SgDeclarationStatement *sgdecl = isSgDeclarationStatement(node);

     if( sgdecl != NULL )
     {
       SgFunctionDeclaration *fd = isSgFunctionDeclaration(node);

       if( fd != NULL )
       {
         if( sgdecl->get_firstNondefiningDeclaration() == NULL )
         {
           output->addOutput( new CompassAnalyses::FunctionDefinitionPrototype::CheckerOutput( node, fd->get_name().getString().c_str() ) );
         }
       } //if( fd != NULL )
     }
   } //End of the visit function.
   
