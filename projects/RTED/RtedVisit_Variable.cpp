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

VariableTraversal::VariableTraversal(RtedTransformation* t)  {
   transf = t;
   isRightBranchOfBinaryOp = false;
   binaryOp = new std::vector<SgBinaryOp*>();
   leftOfbinaryOp = new std::vector<SgExpression*>();
   rightOfbinaryOp = new std::vector<SgExpression*>();
   ROSE_ASSERT(transf);
   ROSE_ASSERT(binaryOp);
   ROSE_ASSERT(leftOfbinaryOp);
   ROSE_ASSERT(rightOfbinaryOp);
};

bool VariableTraversal::isInterestingAssignNode (SgNode* exp) {
      if (		isSgAssignOp( exp )            || isSgAndAssignOp( exp )
            || isSgDivAssignOp( exp )            || isSgIorAssignOp( exp )
            || isSgLshiftAssignOp( exp )            || isSgMinusAssignOp( exp )
            || isSgModAssignOp( exp )            || isSgMultAssignOp( exp )
            || isSgPlusAssignOp( exp )            || isSgPointerAssignOp( exp )
            || isSgRshiftAssignOp( exp )            || isSgXorAssignOp( exp )
			|| isSgAssignInitializer(exp)   
			|| isSgLessThanOp(exp)
) 
	return true;
      else
	cerr << " ----------------------- NODE is not interesting : " << exp->class_name() << endl;
      return false;
}


InheritedAttribute
VariableTraversal::evaluateInheritedAttribute (
      SgNode* astNode,
      InheritedAttribute inheritedAttribute ) {
   ROSE_ASSERT(transf);
   ROSE_ASSERT(binaryOp);
   ROSE_ASSERT(leftOfbinaryOp);
   ROSE_ASSERT(rightOfbinaryOp);

   cerr << "  !!!! >>>> down node  : " << astNode->class_name() << endl;

   if (rightOfbinaryOp && !rightOfbinaryOp->empty()) {
      cerr << " ====================== THe size of rightOfbinaryOp == " << rightOfbinaryOp->size() << endl;
      SgExpression* exp = rightOfbinaryOp->back();
      if  (exp ==astNode) {
         isRightBranchOfBinaryOp = true;
	 cerr << " ================================ Found isRightBranchOfBinaryOp == " << exp->class_name() << endl;
       }
   }

//   printf ("  evaluateInheritedAttr Node...%s  isGlobal %d  isFunction %d  isAssign %d \n",astNode->class_name().c_str(),
//         inheritedAttribute.global,inheritedAttribute.function,inheritedAttribute.isAssignInitializer );
   if (isSgGlobal(astNode))    {
      InheritedAttribute ia(true,
                            inheritedAttribute.function,
                            inheritedAttribute.isAssignInitializer,
                            inheritedAttribute.isArrowExp,
                            inheritedAttribute.isAddressOfOp,
                            inheritedAttribute.isLValue,
                            inheritedAttribute.isReferenceType,
                            inheritedAttribute.isInitializedName,
                            inheritedAttribute.isBinaryOp,
                            inheritedAttribute.isDotExp,
                            inheritedAttribute.isPointerDerefExp);
      printf ("  >>>>> evaluateInheritedAttr isGlobal yes...\n");
      return ia;
   }

   if (isSgFunctionDefinition(astNode))    {
      // The inherited attribute is true iff we are inside a function.
      InheritedAttribute ia(inheritedAttribute.global,
                            true,
                            inheritedAttribute.isAssignInitializer,
                            inheritedAttribute.isArrowExp,
                            inheritedAttribute.isAddressOfOp,
                            inheritedAttribute.isLValue,
                            inheritedAttribute.isReferenceType,
                            inheritedAttribute.isInitializedName,
                            inheritedAttribute.isBinaryOp,
                            inheritedAttribute.isDotExp,
                            inheritedAttribute.isPointerDerefExp);
      printf ("  >>>>> evaluateInheritedAttr isFunctionDefinition yes...\n");
      return ia;
   }

   if (isSgAssignInitializer(astNode))    {
       // The inherited attribute is true iff we are inside a function.
       InheritedAttribute ia(inheritedAttribute.global,
                             inheritedAttribute.function,
                             true,
                             inheritedAttribute.isArrowExp,
                             inheritedAttribute.isAddressOfOp,
                             inheritedAttribute.isLValue,
                             inheritedAttribute.isReferenceType,
                             inheritedAttribute.isInitializedName,
                             inheritedAttribute.isBinaryOp,
                             inheritedAttribute.isDotExp,
                             inheritedAttribute.isPointerDerefExp);
       cerr << "  >>>>> evaluateInheritedAttr isAssignInit yes..." << endl;
       return ia;
    }

   if (isSgArrowExp(astNode))
      return InheritedAttribute (inheritedAttribute.global,
                                   inheritedAttribute.function,
                                   inheritedAttribute.isAssignInitializer,
                                   true,
                                   inheritedAttribute.isAddressOfOp,
                                   inheritedAttribute.isLValue,
                                   inheritedAttribute.isReferenceType,
                                   inheritedAttribute.isInitializedName,
                                   inheritedAttribute.isBinaryOp,
                                   inheritedAttribute.isDotExp,
                                   inheritedAttribute.isPointerDerefExp);

   if (isSgAddressOfOp(astNode))
      return InheritedAttribute (inheritedAttribute.global,
                                   inheritedAttribute.function,
                                   inheritedAttribute.isAssignInitializer,
                                   inheritedAttribute.isArrowExp,
                                   true,
                                   inheritedAttribute.isLValue,
                                   inheritedAttribute.isReferenceType,
                                   inheritedAttribute.isInitializedName,
                                   inheritedAttribute.isBinaryOp,
                                   inheritedAttribute.isDotExp,
                                   inheritedAttribute.isPointerDerefExp);


   if (isSgReferenceType(astNode))
      return InheritedAttribute (inheritedAttribute.global,
                                   inheritedAttribute.function,
                                   inheritedAttribute.isAssignInitializer,
                                   inheritedAttribute.isArrowExp,
                                   inheritedAttribute.isAddressOfOp,
                                   inheritedAttribute.isLValue,
                                   true,
                                   inheritedAttribute.isInitializedName,
                                   inheritedAttribute.isBinaryOp,
                                   inheritedAttribute.isDotExp,
                                   inheritedAttribute.isPointerDerefExp);

   if (isSgInitializedName(astNode))
      return InheritedAttribute (inheritedAttribute.global,
                                   inheritedAttribute.function,
                                   inheritedAttribute.isAssignInitializer,
                                   inheritedAttribute.isArrowExp,
                                   inheritedAttribute.isAddressOfOp,
                                   inheritedAttribute.isLValue,
                                   inheritedAttribute.isReferenceType,
                                   true,
                                   inheritedAttribute.isBinaryOp,
                                   inheritedAttribute.isDotExp,
                                   inheritedAttribute.isPointerDerefExp);

   if (isSgBinaryOp(astNode)) {
     if (isInterestingAssignNode(astNode)) {
      ROSE_ASSERT(binaryOp);
      binaryOp->push_back(isSgBinaryOp(astNode));
      ROSE_ASSERT(isSgBinaryOp(astNode) -> get_lhs_operand());
      ROSE_ASSERT(isSgBinaryOp(astNode) -> get_rhs_operand());
      leftOfbinaryOp->push_back(isSgBinaryOp(astNode) -> get_lhs_operand());
      rightOfbinaryOp->push_back(isSgBinaryOp(astNode) -> get_rhs_operand());
      cerr << "  --------- !!!! >>>>      push : " << isSgBinaryOp(astNode) -> get_rhs_operand()->class_name() << "    parent == " << astNode->class_name() << endl;
      return InheritedAttribute (inheritedAttribute.global,
                                   inheritedAttribute.function,
                                   inheritedAttribute.isAssignInitializer,
                                   inheritedAttribute.isArrowExp,
                                   inheritedAttribute.isAddressOfOp,
                                   inheritedAttribute.isLValue,
                                   inheritedAttribute.isReferenceType,
                                   inheritedAttribute.isInitializedName,
                                   true,
                                   inheritedAttribute.isDotExp,
                                   inheritedAttribute.isPointerDerefExp);
       }
   }

   if (isSgDotExp(astNode))
      return InheritedAttribute (inheritedAttribute.global,
                                   inheritedAttribute.function,
                                   inheritedAttribute.isAssignInitializer,
                                   inheritedAttribute.isArrowExp,
                                   inheritedAttribute.isAddressOfOp,
                                   inheritedAttribute.isLValue,
                                   inheritedAttribute.isReferenceType,
                                   inheritedAttribute.isInitializedName,
                                   inheritedAttribute.isBinaryOp,
                                   true,
                                   inheritedAttribute.isPointerDerefExp);

   if (isSgPointerDerefExp(astNode))
      return InheritedAttribute (inheritedAttribute.global,
                                   inheritedAttribute.function,
                                   inheritedAttribute.isAssignInitializer,
                                   inheritedAttribute.isArrowExp,
                                   inheritedAttribute.isAddressOfOp,
                                   inheritedAttribute.isLValue,
                                   inheritedAttribute.isReferenceType,
                                   inheritedAttribute.isInitializedName,
                                   inheritedAttribute.isBinaryOp,
                                   inheritedAttribute.isDotExp,
                                   true);

   if (isSgExpression(astNode))
      return InheritedAttribute (inheritedAttribute.global,
                                   inheritedAttribute.function,
                                   inheritedAttribute.isAssignInitializer,
                                   inheritedAttribute.isArrowExp,
                                   inheritedAttribute.isAddressOfOp,
                                   isSgExpression(astNode)->isLValue(),
                                   inheritedAttribute.isReferenceType,
                                   inheritedAttribute.isInitializedName,
                                   inheritedAttribute.isBinaryOp,
                                   inheritedAttribute.isDotExp,
                                   inheritedAttribute.isPointerDerefExp);




   return inheritedAttribute;
}



SynthesizedAttribute
VariableTraversal::evaluateSynthesizedAttribute (
      SgNode* astNode,
      InheritedAttribute inheritedAttribute,
      SynthesizedAttributesList childAttributes ) {

    cerr << "  !!!!!!!! >>>> up node  : " << astNode->class_name() << endl;
 //  printf ("      evaluateSynthesizedAttribute Node...%s  isGlobal %d  isFunction %d  isAssign %d \n",astNode->class_name().c_str(),
 //        inheritedAttribute.global,inheritedAttribute.function,inheritedAttribute.isAssignInitializer);
   SynthesizedAttribute localResult =
         std::accumulate(childAttributes.begin(), childAttributes.end(),  false, std::logical_or<bool>());
   if (inheritedAttribute.function == true)
   {
      // Fold up the list of child attributes using logical or, i.e. the local
      // result will be true iff one of the child attributes is true.
      if (isSgFunctionDefinition(astNode) && localResult == true)
      {
         printf ("  >>>>> evaluateSynthesizedAttribute Found a function containing a varRefExp  ...\n");
      }

      if (isSgBinaryOp(astNode) ) {
        cerr <<" ------ popp binaryOp --------- "<< endl;
	if (isInterestingAssignNode(astNode)) {
            if (binaryOp && !binaryOp->empty())        binaryOp->pop_back();
            if (leftOfbinaryOp && !leftOfbinaryOp->empty())  leftOfbinaryOp->pop_back();
            if (rightOfbinaryOp && !rightOfbinaryOp->empty()) rightOfbinaryOp->pop_back();
	    
	    isRightBranchOfBinaryOp=false;
            cerr <<" ------ popp binaryOp setting isRightBranchOfBinaryOp = false --------- "<< endl;
	  }
      }


      if (isSgVarRefExp(astNode)) {
         if (!inheritedAttribute.isArrowExp && !inheritedAttribute.isAddressOfOp
	     //            && !inheritedAttribute.isDotExp && !inheritedAttribute.isPointerDerefExp
         )  {
            bool stopSearch=false;
            if (binaryOp && binaryOp->back()) {
		//!binaryOp->empty()) {
#if 0
//            if (isSgBinaryOp(astNode)) {
//               if (isSgDotExp(astNode) || isSgPointerDerefExp(astNode)) continue;
               SgExpression* left = leftOfbinaryOp->back();
               SgExpression* right = rightOfbinaryOp->back();
               if (  //!isSgVarRefExp(astNode)->isUsedAsLValue()
                     isRightBranchOfBinaryOp
                     && !isSgArrayType( right->get_type() )
                     && !isSgNewExp(right)
                     && !isSgReferenceType( left->get_type() )) {
                  stopSearch=false;
               } else
               stopSearch=true;
               cerr << "============================= DEBUGGING   parent (synthesized) = " << astNode->class_name() << endl;
//               break;
#endif
            }
               // tps : this fails right now because of testcase : run/C/subprogram_call_errors/c_C_2_b  ... ROSE can not handle isUsedAsLValue right now
//              bool lval = isSgVarRefExp(astNode)->isUsedAsLValue();
            //         string name = isSgVarRefExp(astNode)->get_symbol()->get_name().getString();
    //        printf ("\n  $$$$$$$$$$ FOUND isSgVarRefExp : %s  LVALUE? %d...    :: %s \n",name.c_str(), lval,astNode->get_parent()->get_parent()->unparseToString().c_str() );

//            if (!isSgExpression(astNode)->isLValue())
#if 0
            if (lval)
               stopSearch=true;
#endif
#if 1
            if (inheritedAttribute.isAssignInitializer)  {
	      cerr << " =======   inherited attribute :::::::: assignInitializer " << endl;
               SgInitializedName* initName = isSgInitializedName( astNode -> get_parent() ->get_parent()-> get_parent());
               if (initName==NULL)
                  initName = isSgInitializedName( astNode -> get_parent() ->get_parent());
                    if (initName   && isSgReferenceType( initName -> get_type()))
                        stopSearch=true;
            }
#endif
#if 1
            if (stopSearch==false) {
               cout << " @@@@@@@@@ CALLING ADDING Variable access : " << astNode->unparseToString() << endl;
               transf->visit_isSgVarRefExp(isSgVarRefExp(astNode),isRightBranchOfBinaryOp);
            }
#else
            if (stopSearch==false) {
                 // its a plain variable access
               transf->variable_access_varref.push_back(isSgVarRefExp(astNode));
                 cout << " @@@@@@@@@ ADDING Variable access : " << astNode->unparseToString() << "  vec size: " << astNode->get_parent()->unparseToString() << endl;
            }
#endif
         }
      }
   }
   return localResult;
}




void RtedTransformation::visit_isSgVarRefExp(SgVarRefExp* n, bool isRightBranchOfBinaryOp) {
   cout <<"$$$$$ visit_isSgVarRefExp : " << n->unparseToString() <<
         "  in line : " << n->get_file_info()->get_line() << " -------------------------------------" <<endl;

   SgInitializedName *name = n -> get_symbol() -> get_declaration();
   if( name  && !isInInstrumentedFile( name -> get_declaration() )) {
      // we're not instrumenting the file that contains the declaration, so we'll always complain about accesses
      return;
   }

#if 1
   SgNode* parent = isSgVarRefExp(n);
   SgNode* last = parent;
   bool stopSearch=false;
   //cerr << "*********************************** DEBUGGING  " << n->unparseToString() << endl;
   while (!isSgProject(parent)) {
      last=parent;
      parent=parent->get_parent();
    //  cerr << "*********************************** DEBUGGING  parent (isSgVarRefExp) = " << parent->class_name() << endl;
      if( isSgProject(parent))  {
         stopSearch=true;
         break;
      }
      else if (isSgAssignInitializer(parent)) {
#if 0

         SgInitializedName* grandparent = isSgInitializedName( parent -> get_parent() );
         bool condition = !(  grandparent   && isSgReferenceType( grandparent -> get_type()));
         if (!condition)
            stopSearch=true;
#endif
         break;

      }
#if 0
      else if (		isSgAssignOp( parent )            || isSgAndAssignOp( parent )
            || isSgDivAssignOp( parent )            || isSgIorAssignOp( parent )
            || isSgLshiftAssignOp( parent )            || isSgMinusAssignOp( parent )
            || isSgModAssignOp( parent )            || isSgMultAssignOp( parent )
            || isSgPlusAssignOp( parent )            || isSgPointerAssignOp( parent )
            || isSgRshiftAssignOp( parent )            || isSgXorAssignOp( parent )) {
#endif
#if 1
         else if (isSgBinaryOp(parent)) {
         if (isSgDotExp(parent) || isSgPointerDerefExp(parent)) continue;
         SgExpression* left = isSgBinaryOp( parent ) -> get_lhs_operand();
         SgExpression* right = isSgBinaryOp(parent)  ->get_rhs_operand();
         if ( // right == last
	       isRightBranchOfBinaryOp
               &&    !isSgArrayType( right -> get_type() ) &&   !isSgNewExp(right)
               && !isSgReferenceType( left -> get_type() )) {
            stopSearch=false;
         } else
         stopSearch=true;
         cerr << "============================= DEBUGGING   parent (isSgVarRefExp) = " << parent->class_name() << endl;
         break;
      }
#endif
#if 0
      else if ( isSgArrowExp( parent )) {
         stopSearch = true;
         break;
      }
#endif
      else if (isSgExprListExp(parent) && isSgFunctionCallExp(parent->get_parent())) {
         cerr << "$$$$$ Found Function call - lets handle its parameters." << endl;
         SgType* arg_type = isSgExpression(last)->get_type();
         SgType* param_type = NULL;

         // try to determine the parameter type
         SgFunctionDeclaration* fndecl = isSgFunctionCallExp( parent -> get_parent() )-> getAssociatedFunctionDeclaration();
         if( fndecl ) {
            int param_index = -1;
            SgExpressionPtrList& args = isSgExprListExp( parent ) -> get_expressions();
            for( unsigned int i = 0; i < args.size(); ++i ) {
               if( args[ i ] == last ) {
                  param_index = i;
                  break;
               }
            }
            ROSE_ASSERT( param_index > -1 );

            if( (int)fndecl -> get_parameterList() -> get_args().size() > param_index ) {
               SgInitializedName* param  = fndecl -> get_parameterList()-> get_args()[ param_index ];
               if( param )  param_type = param -> get_type();
            }
         }

         if ((   arg_type   && isUsableAsSgArrayType( arg_type ) != NULL )
               || (param_type  && isUsableAsSgReferenceType( param_type ) != NULL ))
            stopSearch=true;
         break;
      }
#if 0
      else if (isSgAddressOfOp(parent)) {
         // consider, e.g.
         // 	int x;
         // 	int* y = &x;
         // it is not necessary for x to be initialized
         stopSearch = true;
         break;
      }
#endif

      else if( isSgWhileStmt( parent )
            || isSgDoWhileStmt( parent )
            || isSgIfStmt( parent )) {

         break;
      }

      else if( isSgForStatement( parent )) {
         SgForStatement* for_stmt = isSgForStatement( parent );
         // Capture for( int i = 0;
         vector< SgNode* > initialized_names = NodeQuery::querySubTree( for_stmt -> get_for_init_stmt(), V_SgInitializedName );

         // Capture int i; for( i = 0;
         vector< SgNode* > init_var_refs = NodeQuery::querySubTree( for_stmt -> get_for_init_stmt(), V_SgVarRefExp );
         for(vector< SgNode* >::iterator i = init_var_refs.begin(); i != init_var_refs.end();  ++i) {
            initialized_names.push_back(
                  // map the var refs to their initialized names
                  isSgVarRefExp( *i ) -> get_symbol() -> get_declaration()
            );
         }

         if( find( initialized_names.begin(), initialized_names.end(), name )  != initialized_names.end() ) {
            // no need to check the ref
            stopSearch = true;
         }
         // either way, no need to keep going up the AST
         break;
      }  else {
         //cerr << "*********************************** DEBUGGING   parent (else) = " << parent->class_name() << endl;
      }
   } //while

   if (stopSearch==false) {
      // its a plain variable access
      variable_access_varref.push_back(n);
      cerr << " @@@@@@@@@ ADDING Variable access : " << n->unparseToString() << "  parent: " << n->get_parent()->unparseToString() << endl;
   }

#endif
}

#endif



