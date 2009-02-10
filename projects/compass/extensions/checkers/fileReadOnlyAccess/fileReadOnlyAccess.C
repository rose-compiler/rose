// File Read Only Access
// Author: Gary Yuan
// Date: 03-January-2008

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_FILE_READ_ONLY_ACCESS_H
#define COMPASS_FILE_READ_ONLY_ACCESS_H

namespace CompassAnalyses
   { 
     namespace FileReadOnlyAccess
        { 
        /*! \brief File Read Only Access: Add your description here 
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

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n){ this->traverse(n, preorder); }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_FILE_READ_ONLY_ACCESS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// File Read Only Access Analysis
// Author: Gary Yuan
// Date: 03-January-2008

#include "compass.h"
// #include "fileReadOnlyAccess.h"

namespace CompassAnalyses
   { 
     namespace FileReadOnlyAccess
        { 
          const std::string checkerName      = "FileReadOnlyAccess";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that all standard C/C++ file I/O is read-only.";
          const std::string longDescription  = "Long description not written yet!";
        } //End of namespace FileReadOnlyAccess.
   } //End of namespace CompassAnalyses.

CompassAnalyses::FileReadOnlyAccess::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::FileReadOnlyAccess::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FileReadOnlyAccess.YourParameter"]);


   }

void
CompassAnalyses::FileReadOnlyAccess::Traversal::
visit(SgNode* node)
   {
     SgFunctionCallExp *fcall = isSgFunctionCallExp(node);
     SgVariableDeclaration *vdecl = isSgVariableDeclaration(node);

     if( fcall != NULL )
     {
       SgFunctionRefExp *fref = isSgFunctionRefExp(fcall->get_function());

       if( fref != NULL )
       {
         if( fref->get_symbol()->get_name().getString() == "fopen" )
         {
           const SgExpressionPtrList arguments = fcall->get_args()->get_expressions();
           ROSE_ASSERT( arguments.size() == 2 );

           SgExpressionPtrList::const_iterator itr = arguments.begin(); itr++;

           SgStringVal *mode = isSgStringVal(*itr);

	   // tps (18Dec2008) : added the (mode==NULL) condition because checker fails.
	   if (mode==NULL) return;
           ROSE_ASSERT( mode != NULL );

           if( mode->get_value() != "r" && mode->get_value() != "rb" )
           {
             output->addOutput( new CheckerOutput(fcall) );
           } //if( mode->get_value() != "r" || mode->get_value() != "rb" )
         } //if( fref->get_symbol()->get_name().getString() == "fopen" )
       } //if( fref != NULL )
     } //if( fcall != NULL )

     if( vdecl != NULL )
     {
       SgVariableDefinition *vdef = vdecl->get_definition();
       ROSE_ASSERT( vdef != NULL );

       SgNamedType *vname = isSgNamedType( vdef->get_type() );

       if( vname != NULL )
       {
         std::string vnameString( vname->get_name().getString() );

         if( vnameString == "fstream" || vnameString == "ofstream" )
         {
           output->addOutput( new CheckerOutput(vdecl) );
         } //if( vnameString == "fstream" || vnameString == "ofstream" )
       } //if( vname != NULL )
     } //if( vdecl != NULL )

     return;
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::FileReadOnlyAccess::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::FileReadOnlyAccess::Traversal(params, output);
}

extern const Compass::Checker* const fileReadOnlyAccessChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::FileReadOnlyAccess::checkerName,
        CompassAnalyses::FileReadOnlyAccess::shortDescription,
        CompassAnalyses::FileReadOnlyAccess::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
