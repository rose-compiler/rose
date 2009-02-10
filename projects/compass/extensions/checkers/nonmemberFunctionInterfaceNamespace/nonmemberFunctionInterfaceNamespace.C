// Nonmember Function Interface Namespace
// Author: Gergo  Barany
// Date: 31-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NONMEMBER_FUNCTION_INTERFACE_NAMESPACE_H
#define COMPASS_NONMEMBER_FUNCTION_INTERFACE_NAMESPACE_H

namespace CompassAnalyses
   { 
     namespace NonmemberFunctionInterfaceNamespace
        { 
        /*! \brief Nonmember Function Interface Namespace: Add your description here 
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
                 SgNamespaceDefinitionStatement *enclosingNamespace(SgNode *p);
                 bool sameNamespace(SgNamespaceDefinitionStatement *a,
                                    SgNamespaceDefinitionStatement *b);
                 bool stdNamespace(SgNamespaceDefinitionStatement *n);
                 bool checkTypeNamespace(SgType *type,
                                         SgNamespaceDefinitionStatement *n);

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_NONMEMBER_FUNCTION_INTERFACE_NAMESPACE_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Nonmember Function Interface Namespace Analysis
// Author: Gergo Barany
// Date: 31-July-2007

#include "compass.h"
// #include "nonmemberFunctionInterfaceNamespace.h"
#include <cstring>

namespace CompassAnalyses
   { 
     namespace NonmemberFunctionInterfaceNamespace
        { 
          const std::string checkerName      = "NonmemberFunctionInterfaceNamespace";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Friend function or operator "
            "for a class in a different namespace.";
          const std::string longDescription  = "A function that is part of a "
            "type T's nonmember function interface (all friend functions and "
            "overloaded operators using that type) is in a different namespace "
            "from T.";
        } //End of namespace NonmemberFunctionInterfaceNamespace.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NonmemberFunctionInterfaceNamespace::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NonmemberFunctionInterfaceNamespace::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NonmemberFunctionInterfaceNamespace.YourParameter"]);
   }

void
CompassAnalyses::NonmemberFunctionInterfaceNamespace::Traversal::
visit(SgNode* node)
   {
     if (SgFunctionDeclaration *fd = isSgFunctionDeclaration(node))
     {
       if (fd->get_declarationModifier().isFriend())
       {
         // find the namespace the class is embedded in; the class is the
         // function declaration's parent
         SgNamespaceDefinitionStatement *classNamespace
           = enclosingNamespace(fd->get_parent());

         // find the namespace the function is declared in; this can be found
         // by searching from the function declaration's scope!
         SgNamespaceDefinitionStatement *funcNamespace
           = enclosingNamespace(fd->get_scope());

         if (!sameNamespace(classNamespace, funcNamespace))
           output->addOutput(new CheckerOutput(fd));
       }
       else if (fd->get_specialFunctionModifier().isOperator()
             && !isSgClassDefinition(fd->get_scope()))
       {
         // see if this nonmember operator's named types all come from the
         // same namespace (if any) OR from ::std (the latter for operator<<
         // used with ostreams etc.)
         SgNamespaceDefinitionStatement *funcNamespace
           = enclosingNamespace(fd->get_scope());

         SgFunctionType *ft = fd->get_type();
         if (!checkTypeNamespace(ft->get_return_type(), funcNamespace))
         {
           output->addOutput(new CheckerOutput(fd));
         }
         else
         {
           SgTypePtrList &argTypes = ft->get_arguments();
           SgTypePtrList::iterator arg;
           for (arg = argTypes.begin(); arg != argTypes.end(); ++arg)
           {
             if (!checkTypeNamespace(*arg, funcNamespace))
             {
               output->addOutput(new CheckerOutput(fd));
               break;
             }
           }
         }
       }
     }
   } //End of the visit function.

SgNamespaceDefinitionStatement *
CompassAnalyses::NonmemberFunctionInterfaceNamespace::Traversal::
enclosingNamespace(SgNode *p)
{
  while (p != NULL && !isSgNamespaceDefinitionStatement(p))
    p = p->get_parent();
  return isSgNamespaceDefinitionStatement(p);
}

bool
CompassAnalyses::NonmemberFunctionInterfaceNamespace::Traversal::
sameNamespace(SgNamespaceDefinitionStatement *a, SgNamespaceDefinitionStatement *b)
{
  // "same" namespace if both are no namespace at all, or both have the same name
  return (a == NULL && b == NULL)
      || (a != NULL && b != NULL)
        && std::strcmp(a->get_qualified_name().str(),
                       b->get_qualified_name().str()) == 0;
}

bool
CompassAnalyses::NonmemberFunctionInterfaceNamespace::Traversal::
stdNamespace(SgNamespaceDefinitionStatement *n)
{
  return n != NULL && std::strcmp(n->get_qualified_name().str(), "::std") == 0;
}

bool
CompassAnalyses::NonmemberFunctionInterfaceNamespace::Traversal::
checkTypeNamespace(SgType *type, SgNamespaceDefinitionStatement *n)
{
  SgNamedType *namedType
    = isSgNamedType(type->findBaseType());
  if (namedType)
  {
    SgNamespaceDefinitionStatement *ntNamespace
      = enclosingNamespace(namedType->get_declaration());

    if (!sameNamespace(ntNamespace, n) && !stdNamespace(ntNamespace))
      return false;
  }
  return true;
}

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NonmemberFunctionInterfaceNamespace::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NonmemberFunctionInterfaceNamespace::Traversal(params, output);
}

extern const Compass::Checker* const nonmemberFunctionInterfaceNamespaceChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NonmemberFunctionInterfaceNamespace::checkerName,
        CompassAnalyses::NonmemberFunctionInterfaceNamespace::shortDescription,
        CompassAnalyses::NonmemberFunctionInterfaceNamespace::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
