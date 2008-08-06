// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Data Member Access Analysis
// Author: Gergo Barany
// Date: 24-July-2007

#include "compass.h"
#include "dataMemberAccess.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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
