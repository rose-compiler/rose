// Author: Dan Quinlan

// Example program to report use of class member variables in lambda capture list.
// It is using the InheritedAttribute and SynthesizedAttribute to permit the next version 
// to support the required transformation to fix the problem that this current version 
// only detects.

#include "rose.h"

// Inherited attribute (see ROSE Tutorial (Chapter 9)).
class InheritedAttribute
   {
     public:
          InheritedAttribute();
   };

// Constructor (not really needed)
InheritedAttribute::InheritedAttribute()
   {
   }

// Synthesized attribute (see ROSE Tutorial (Chapter 9)).
class SynthesizedAttribute
   {
     public:
          SynthesizedAttribute();
   };

// Constructor
SynthesizedAttribute::SynthesizedAttribute()
   {
   }

class Traversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {
     public:
          Traversal();

       // Functions required
          InheritedAttribute   evaluateInheritedAttribute   ( SgNode* astNode, InheritedAttribute inheritedAttribute );
          SynthesizedAttribute evaluateSynthesizedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );
   };


InheritedAttribute
Traversal::evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute )
   {
     SgLambdaCapture* lambdaCapture = isSgLambdaCapture(astNode);
     if (lambdaCapture != NULL)
        {
          SgExpression* captureVariable = lambdaCapture->get_capture_variable();
          if (captureVariable != NULL)
             {
               SgThisExp* thisExp = isSgThisExp(captureVariable);
               if (thisExp != NULL)
                  {
                    printf ("Identified use of C++ \"this\" expression in lambda capture list: \n");

                    std::string filename  = lambdaCapture->get_file_info()->get_filenameString();
                    int lineNumber   = lambdaCapture->get_file_info()->get_line();
                    int columnNumber = lambdaCapture->get_file_info()->get_col();

                    printf ("   --- location: \n");
                    printf ("   --- --- file = %s \n",filename.c_str());
                    printf ("   --- --- line = %d column = %d \n",lineNumber,columnNumber);
                  }
             }
        }

  // This will call the default copy constructor for InheritedAttribute.
     return inheritedAttribute;
   }


SynthesizedAttribute
Traversal::evaluateSynthesizedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute, SynthesizedAttributesList childAttributes )
   {
     SynthesizedAttribute localResult;

  // printf ("evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());

     return localResult;
   }


Traversal::Traversal()
   {
   }

int
main ( int argc, char* argv[] )
   {
  // Get the command line so that we can add an option.
     std::vector<std::string> commandLine = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);

  // Use -rose:no_optimize_flag_for_frontend option to avoid issue with builtin functions not yet supported in ROSE.
  // This allows more general support of the latest versions of compilers that have additional builtin functions.
  // NOTE: This does not effect optimizations at the backend compiler.
     commandLine.push_back("-rose:no_optimize_flag_for_frontend");

  // CmdOptions::GetInstance()->SetOptions(argc, argv);
     SgProject* project = frontend(commandLine);
     ROSE_ASSERT (project != NULL);

  // Build the inherited attribute
     InheritedAttribute inheritedAttribute;

  // Define the traversal
     Traversal astTraversal;

  // Call the traversal starting at the project (root) node of the AST
     astTraversal.traverseInputFiles(project,inheritedAttribute);

  // This tool only does analysis so we can compile the original source code.
     return backendCompilesUsingOriginalInputFile(project);
   }


