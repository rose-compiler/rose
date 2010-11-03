// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
#include "rose.h"

// DQ (2/9/2010): Testing use of ROSE to compile ROSE.
#ifndef USE_ROSE

#include <algorithm>
#include <functional>
#include <numeric>

#include <string>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

#include "RtedVisit.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

VariableTraversal::VariableTraversal(RtedTransformation* t) {
   transf = t;
   rightOfbinaryOp = new std::vector<SgExpression*>();
   ROSE_ASSERT(rightOfbinaryOp);
   for_stmt = new std::vector<SgForStatement*>();
}

bool VariableTraversal::isInitializedNameInForStatement(SgForStatement* for_stmt, SgInitializedName* name) {
   // Capture for( int i = 0;
   vector<SgNode*> initialized_names = NodeQuery::querySubTree(for_stmt -> get_for_init_stmt(), V_SgInitializedName);
   // Capture int i; for( i = 0;
   vector<SgNode*> init_var_refs = NodeQuery::querySubTree(for_stmt -> get_for_init_stmt(), V_SgVarRefExp);
   for (vector<SgNode*>::iterator i = init_var_refs.begin(); i != init_var_refs.end(); ++i)
      initialized_names.push_back(isSgVarRefExp(*i) -> get_symbol() -> get_declaration());

   if (find(initialized_names.begin(), initialized_names.end(), name) != initialized_names.end())
      return true;
   return false;
}

bool VariableTraversal::isRightOfBinaryOp(SgNode* astNode) {
   SgNode* temp = astNode;
   while (!isSgProject(temp)) {
      if (temp->get_parent() && isSgBinaryOp(temp->get_parent()) && !(isSgDotExp(temp->get_parent()) || isSgPointerDerefExp(
            temp->get_parent())))
         if (isSgBinaryOp(temp->get_parent())->get_rhs_operand() == temp) {
            return true;
         } else
            break;
      temp = temp->get_parent();
   }
   return false;
}

InheritedAttribute VariableTraversal::evaluateInheritedAttribute(SgNode* astNode, InheritedAttribute inheritedAttribute) {

   transf->visit(astNode);
   if (isSgFunctionDefinition(astNode)) {
      // ------------------------------ visit isSgFunctionDefinition ----------------------------------------------
   #if 0
         if (isSgFunctionDefinition(astNode)) {
            transf->visit_checkIsMain( astNode);
            transf->function_definitions.push_back(isSgFunctionDefinition(astNode));
         }
   #endif
      return InheritedAttribute(true, inheritedAttribute.isAssignInitializer, inheritedAttribute.isArrowExp,
            inheritedAttribute.isAddressOfOp, inheritedAttribute.isForStatement, inheritedAttribute.isBinaryOp);
   }

   if (isSgAssignInitializer(astNode)) {
      return InheritedAttribute(inheritedAttribute.function, true, inheritedAttribute.isArrowExp,
            inheritedAttribute.isAddressOfOp, inheritedAttribute.isForStatement, inheritedAttribute.isBinaryOp);
   }

   if (isSgArrowExp(astNode))
      return InheritedAttribute(inheritedAttribute.function, inheritedAttribute.isAssignInitializer, true,
            inheritedAttribute.isAddressOfOp, inheritedAttribute.isForStatement, inheritedAttribute.isBinaryOp);

   if (isSgAddressOfOp(astNode))
      return InheritedAttribute(inheritedAttribute.function, inheritedAttribute.isAssignInitializer,
            inheritedAttribute.isArrowExp, true, inheritedAttribute.isForStatement, inheritedAttribute.isBinaryOp);

   if (isSgForStatement(astNode)) {
      for_stmt->push_back(isSgForStatement(astNode));
      return InheritedAttribute(inheritedAttribute.function, inheritedAttribute.isAssignInitializer,
            inheritedAttribute.isArrowExp, inheritedAttribute.isAddressOfOp, true, inheritedAttribute.isBinaryOp);
   }

   if (isSgBinaryOp(astNode) && !inheritedAttribute.isArrowExp && !inheritedAttribute.isAddressOfOp && !isSgDotExp(astNode)
         && !isSgPointerDerefExp(astNode)) {
      ROSE_ASSERT(isSgBinaryOp(astNode) -> get_rhs_operand());
      rightOfbinaryOp->push_back(isSgBinaryOp(astNode) -> get_rhs_operand());
      return InheritedAttribute(inheritedAttribute.function, inheritedAttribute.isAssignInitializer,
            inheritedAttribute.isArrowExp, inheritedAttribute.isAddressOfOp, inheritedAttribute.isForStatement, true);
   }

   return inheritedAttribute;
}

SynthesizedAttribute VariableTraversal::evaluateSynthesizedAttribute(SgNode* astNode, InheritedAttribute inheritedAttribute,
      SynthesizedAttributesList childAttributes) {
   SynthesizedAttribute localResult = std::accumulate(childAttributes.begin(), childAttributes.end(), false,
         std::logical_or<bool>());


   if (inheritedAttribute.function == true) {
      if (isSgForStatement(astNode))
         for_stmt->pop_back();// = NULL;

      bool isRightBranchOfBinary = isRightOfBinaryOp(astNode);
      if (isSgBinaryOp(astNode) && !inheritedAttribute.isArrowExp && !inheritedAttribute.isAddressOfOp && !isSgDotExp(astNode)
            && !isSgPointerDerefExp(astNode) && rightOfbinaryOp && !rightOfbinaryOp->empty())
         rightOfbinaryOp->pop_back();

      // ------------------------------ visit isSgVarRefExp ----------------------------------------------
      if (isSgVarRefExp(astNode)) {
         SgInitializedName *name = isSgVarRefExp(astNode) -> get_symbol() -> get_declaration();
         if (name && !transf->isInInstrumentedFile(name -> get_declaration())) {
            return localResult;
         }
         bool stopSearch = false;

         SgForStatement* fstmt = NULL;
         if (!for_stmt->empty())
            fstmt = for_stmt->back();

         if (fstmt)
            stopSearch = isInitializedNameInForStatement(fstmt, name);
         if (!inheritedAttribute.isArrowExp && !inheritedAttribute.isAddressOfOp) {
            if (rightOfbinaryOp && !rightOfbinaryOp->empty()) {
               if (isRightBranchOfBinary && !isSgArrayType(rightOfbinaryOp->back()->get_type()) && !isSgNewExp(
                     rightOfbinaryOp->back()) && !isSgReferenceType(
                     isSgBinaryOp(rightOfbinaryOp->back()->get_parent())->get_lhs_operand()->get_type())) {
                  stopSearch = false;
               } else
                  stopSearch = true;
            }

            if (inheritedAttribute.isAssignInitializer) {
               SgInitializedName* initName = isSgInitializedName(astNode -> get_parent() ->get_parent()-> get_parent());
               if (initName == NULL)
                  initName = isSgInitializedName(astNode -> get_parent() ->get_parent());
               if (initName && isSgReferenceType(initName -> get_type()))
                  stopSearch = true;
            }

            SgExprListExp* exprl = isSgExprListExp(astNode->get_parent());
            if (isSgExprListExp(exprl) && isSgFunctionCallExp(exprl->get_parent())) {
               SgType* param_type = NULL;
               // try to determine the parameter type
               SgFunctionDeclaration* fndecl = isSgFunctionCallExp(exprl->get_parent())-> getAssociatedFunctionDeclaration();
               if (fndecl) {
                  int param_index = -1;
                  SgExpressionPtrList& args = exprl->get_expressions();
                  for (unsigned int i = 0; i < args.size(); ++i) {
                     if (args[i] == astNode) {
                        param_index = i;
                        break;
                     }
                  }
                  ROSE_ASSERT( param_index > -1 );

                  if ((int) fndecl -> get_parameterList() -> get_args().size() > param_index
                        && fndecl -> get_parameterList()-> get_args()[param_index])
                     param_type = fndecl -> get_parameterList()-> get_args()[param_index] -> get_type();
               }

               if ((isSgExpression(astNode)->get_type() && transf->isUsableAsSgArrayType(isSgExpression(astNode)->get_type())
                     != NULL) || (param_type && transf->isUsableAsSgReferenceType(param_type) != NULL))
                  stopSearch = true;
            }

            if (stopSearch == false) {
               // its a plain variable access
               transf->variable_access_varref.push_back(isSgVarRefExp(astNode));
               if (RTEDDEBUG())
                  cout << " @@@@@@@@@ ADDING Variable access : " << astNode->unparseToString() << "  vec size: "
                        << astNode->get_parent()->unparseToString() << endl;
            }
         }
      }

#if 0
      // ------------------------------ visit Variable Declarations ----------------------------------------------
      if (isSgVariableDeclaration(astNode) && !isSgClassDefinition(isSgVariableDeclaration(astNode) -> get_parent())) {
         Rose_STL_Container<SgInitializedName*> vars = isSgVariableDeclaration(astNode)->get_variables();
         for (Rose_STL_Container<SgInitializedName*>::const_iterator it = vars.begin();it!=vars.end();++it) {
            SgInitializedName* initName = *it;
            ROSE_ASSERT(initName);
            if( isSgReferenceType( initName -> get_type() ))
            continue;
            transf->variable_declarations.push_back(initName);
         }
      }


      // ------------------------------ visit isSgInitializedName ----------------------------------------------
      if (isSgInitializedName(astNode)) {
         // STACK ARRAY : lets see if we assign an array here
         ROSE_ASSERT(isSgInitializedName(astNode)->get_typeptr());
         SgArrayType* array = isSgArrayType(isSgInitializedName(astNode)->get_typeptr());
         SgNode* gp = astNode -> get_parent() -> get_parent();
         // something like:
         //  struct type { int before; char c[ 10 ]; int after; }
         // does not need a createarray call, as the type is registered and any array
         // information will be tracked when variables of that type are created
         //
         // ignore arrays in parameter lists as they're actually pointers, not stack arrays
         if ( array  && !( isSgClassDefinition( gp )) && !( isSgFunctionDeclaration( gp ) )) {
            RTedArray* arrayRted = new RTedArray(true, isSgInitializedName(astNode), NULL, false);
            transf->populateDimensions( arrayRted, isSgInitializedName(astNode), array );
            transf->create_array_define_varRef_multiArray_stack[isSgInitializedName(astNode)] = arrayRted;
         }
      }
#endif

   }
   return localResult;
}

#endif

