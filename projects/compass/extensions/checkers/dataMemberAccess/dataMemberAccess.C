// Data Member Access
// Author: Gergo  Barany
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_DATA_MEMBER_ACCESS_H
#define COMPASS_DATA_MEMBER_ACCESS_H

namespace CompassAnalyses
   { 
     namespace DataMemberAccess
        { 
        /*! \brief Data Member Access: Add your description here 
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

// COMPASS_DATA_MEMBER_ACCESS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Data Member Access Analysis
// Author: Gergo Barany
// Date: 24-July-2007

#include "compass.h"
// #include "dataMemberAccess.h"

namespace CompassAnalyses
   { 
     namespace DataMemberAccess
        { 
          const std::string checkerName      = "DataMemberAccess";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Class definition has a mix of public/nonpublic data members.";
          const std::string longDescription  = "A class definition contains a"
            " mix of public and protected or private data members, i.e. it"
            " hides some, but not all, of its data.";
        } //End of namespace DataMemberAccess.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DataMemberAccess::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DataMemberAccess::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
   }

void
CompassAnalyses::DataMemberAccess::Traversal::
visit(SgNode* n)
   { 
      SgClassDefinition *classdef = isSgClassDefinition(n);
      if (classdef != NULL)
      {
          int pub = 0, prot = 0, priv = 0;
          SgDeclarationStatementPtrList members = classdef->get_members();
          SgDeclarationStatementPtrList::iterator member;
          for (member = members.begin(); member != members.end(); ++member)
          {
              SgVariableDeclaration *vardecl = isSgVariableDeclaration(*member);
              if (vardecl != NULL)
              {
                  SgAccessModifier &mod = vardecl->get_declarationModifier().get_accessModifier();
                  if (mod.isPublic())
                      pub++;
                  else if (mod.isProtected())
                      prot++;
                  else if (mod.isPrivate())
                      priv++;
              }
          }
          if (pub != 0 && prot + priv != 0)
              output->addOutput(new CheckerOutput(n));
      }
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::DataMemberAccess::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::DataMemberAccess::Traversal(params, output);
}

extern const Compass::Checker* const dataMemberAccessChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::DataMemberAccess::checkerName,
        CompassAnalyses::DataMemberAccess::shortDescription,
        CompassAnalyses::DataMemberAccess::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
