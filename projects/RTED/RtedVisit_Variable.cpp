// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
#include "rose.h"

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
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


InheritedAttribute
VariableTraversal::evaluateInheritedAttribute (
      SgNode* astNode,
      InheritedAttribute inheritedAttribute ) {
   printf ("  evaluateInheritedAttr Node...%s  isGlobal %d  isFunction %d \n",astNode->class_name().c_str(),
         inheritedAttribute.global,inheritedAttribute.function );
   if (isSgGlobal(astNode))    {
      InheritedAttribute ia(true,inheritedAttribute.function);
      printf ("  >>>>> evaluateInheritedAttr isGlobal yes...\n");
      return ia;
   }

   if (isSgFunctionDefinition(astNode))    {
      // The inherited attribute is true iff we are inside a function.
      InheritedAttribute ia(inheritedAttribute.global, true);
      printf ("  >>>>> evaluateInheritedAttr isFunctionDefinition yes...\n");
      return ia;
   }

   return inheritedAttribute;
}

SynthesizedAttribute
VariableTraversal::evaluateSynthesizedAttribute (
      SgNode* astNode,
      InheritedAttribute inheritedAttribute,
      SynthesizedAttributesList childAttributes ) {
   printf ("      evaluateSynthesizedAttribute Node...%s\n",astNode->class_name().c_str());
   if (inheritedAttribute.function == true)
   {
      // Fold up the list of child attributes using logical or, i.e. the local
      // result will be true iff one of the child attributes is true.
      SynthesizedAttribute localResult =
            std::accumulate(childAttributes.begin(), childAttributes.end(),  false, std::logical_or<bool>());
      if (isSgFunctionDefinition(astNode) && localResult == true)
      {
         printf ("  >>>>> evaluateSynthesizedAttribute Found a function containing a varRefExp ...\n");
      }
      if (isSgVarRefExp(astNode))
      {
         transf->visit_isSgVarRefExp(isSgVarRefExp(astNode));
         localResult = true;
         printf ("  >>>>> evaluateSynthesizedAttribute isSgVarRefExp...\n");
      }
      return localResult;
   }
}




// FIXME 2: This introduces superfluous calls to access var for dot expressions
// 	e.g., in the following
// 		a = *(my_struct.field);
// 	it is necessary to read &*(my_struct.field), but we will do so twice because
// 	of the two var refs ('my_struct', and 'field')
void RtedTransformation::visit_isSgVarRefExp(SgVarRefExp* n) {
   cerr <<"\n$$$$$ visit_isSgVarRefExp : " << n->unparseToString() <<
         "  in line : " << n->get_file_info()->get_line() << " -------------------------------------" <<endl;

   SgInitializedName *name = n -> get_symbol() -> get_declaration();
   if( name  && !isInInstrumentedFile( name -> get_declaration() )) {
      // we're not instrumenting the file that contains the declaration, so
      // we'll always complain about accesses
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
      cerr << "*********************************** DEBUGGING  parent (loop) = " << parent->class_name() << endl;
      if( isSgProject(parent))
      { // do nothing
         stopSearch=true;
         //cerr << "*********************************** DEBUGGING   parent = (project) " << parent->class_name() << endl;
         break;
      }
      else if (isSgAssignInitializer(parent)) {

         SgInitializedName* grandparent = isSgInitializedName( parent -> get_parent() );
         // make sure that we came from the right hand side of the assignment
         SgExpression* right = isSgAssignInitializer(parent)->get_operand();
         if (    right==last
               && !(
                     // consider, e.g:
                     //  int& ref = x;
                     // which is not a read of x
                     grandparent
                     && isSgReferenceType( grandparent -> get_type())))
            variable_access_varref.push_back(n);
         stopSearch=true;
         cerr << " $$$$ *********************************** DEBUGGING   parent (assigniniit) = " << parent->class_name() << endl;
         break;
      }
      else if (		isSgAssignOp( parent )
            || isSgAndAssignOp( parent )
            || isSgDivAssignOp( parent )
            || isSgIorAssignOp( parent )
            || isSgLshiftAssignOp( parent )
            || isSgMinusAssignOp( parent )
            || isSgModAssignOp( parent )
            || isSgMultAssignOp( parent )
            || isSgPlusAssignOp( parent )
            || isSgPointerAssignOp( parent )
            || isSgRshiftAssignOp( parent )
            || isSgXorAssignOp( parent )) {

         SgExpression* left = isSgBinaryOp( parent ) -> get_lhs_operand();
         // make sure that we came from the right hand side of the assignment
         SgExpression* right = isSgBinaryOp(parent)->get_rhs_operand();
         // consider
         //		int arr[2];
         //		int *x = arr;
         // the assignment is not a var ref of arr since arr's address is statically
         // determined
         if (    right == last &&
               !isSgArrayType( right -> get_type() ) &&
               !isSgNewExp(right)
               // consider:
               //      int& s = x;
               // which is not a read of x
               && !isSgReferenceType( left -> get_type() )) {
            cerr <<"$$$$$ Adding variable_access_varref (assignOp): " << n->unparseToString() <<
                  "   right hand side type: " << right->get_type()->class_name() <<
                  "   right exp: " << right->class_name() << endl;
            variable_access_varref.push_back(n);
            //stopSearch=true;
         } else {
            // tps (09/15/2009): added this
            cerr <<"$$$$$ Unhandled case (AssignOp) : " << parent->unparseToString() << endl;
            cerr <<"$$$$$ right : " << right->unparseToString() <<
                  "    last : " << last->unparseToString() << endl;
            //exit(1);
         }
         stopSearch=true;
         //cerr << "*********************************** DEBUGGING   parent (assign) = " << parent->class_name() << endl;
         break;
      }
      else if (isSgPointerDerefExp(parent)) {
      } else if ( isSgArrowExp( parent )) {
         stopSearch = true;
         break;
      }
      else if (isSgExprListExp(parent) && isSgFunctionCallExp(parent->get_parent())) {
         cerr << "$$$$$ Found Function call - lets handle its parameters." << endl;
         SgType* arg_type = isSgExpression(last)->get_type();
         SgType* param_type = NULL;

         // try to determine the parameter type
         SgFunctionDeclaration* fndecl
         = isSgFunctionCallExp( parent -> get_parent() )
         -> getAssociatedFunctionDeclaration();
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

            // If we're in C, the function declaration could be just about
            // anything.  In particular, it's not guaranteed that it has as many
            // parameters as our callsite has arguments.
            if( (int)fndecl -> get_parameterList() -> get_args().size() > param_index ) {
               SgInitializedName* param
               = fndecl -> get_parameterList()
               -> get_args()[ param_index ];
               if( param )
                  param_type = param -> get_type();
            }
         }

         if (      (   arg_type
               // Pointers are passed as pointers
               && isUsableAsSgArrayType( arg_type ) != NULL )
               || (param_type
                     // References do not have to be initialized.  They can be
                     // initialized in the function body
                     && isUsableAsSgReferenceType( param_type ) != NULL ))
            stopSearch=true;
         break;
      } else if (isSgAddressOfOp(parent)) {
         // consider, e.g.
         // 	int x;
         // 	int* y = &x;
         // it is not necessary for x to be initialized
         stopSearch = true;
         break;
      } else if( isSgWhileStmt( parent )
            || isSgDoWhileStmt( parent )
            || isSgIfStmt( parent )) {
         // FIXME 1: while, getSurroundingStatement( n ) will get the
         // scopestatement, but we also need to append to the body.  consider, e.g.
         // 	int *x = ...
         // 	while( *x > 2 ) { x = NULL; }'
         //
         // perhaps only true for pointers
         //
         // FIXME 1: dowhile has an additional problem with false positives because
         // of checks before the dowhile rather than in the body. consider:
         //    int *x = NULL;
         //    do {
         //        *x = 200;
         //    } while( *x > 0 );
         break;
      } else if( isSgForStatement( parent )) {
         SgForStatement* for_stmt = isSgForStatement( parent );

         // Capture for( int i = 0;
         vector< SgNode* > initialized_names
         = NodeQuery::querySubTree( for_stmt -> get_for_init_stmt(), V_SgInitializedName );

         // Capture int i; for( i = 0;
         vector< SgNode* > init_var_refs
         = NodeQuery::querySubTree( for_stmt -> get_for_init_stmt(), V_SgVarRefExp );
         for(      vector< SgNode* >::iterator i = init_var_refs.begin();
               i != init_var_refs.end();
               ++i) {

            initialized_names.push_back(
                  // map the var refs to their initialized names
                  isSgVarRefExp( *i ) -> get_symbol() -> get_declaration()
            );
         }

         // FIXME 1: This isn't true for pointers -- in general checks need to be
         // added to for's body.  consider, e.g.:
         // 	int *x = ...
         // 	for( ; *x > 2; ) { x = NULL; }'
         if(   find( initialized_names.begin(), initialized_names.end(), name )
               != initialized_names.end() ) {
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
      cerr << " @@@@@@@@@ ADDING Variable access : " << n->unparseToString() << endl;
      variable_access_varref.push_back(n);
   }
#else
   bool donothing=false;
   SgNode* parent = n->get_parent();
   if (isSgArrowExp(parent))
      donothing=true;
   if (isUsedAsLvalue(n))
      donothing=true;
   if (isSgExprListExp(parent) && isSgFunctionCallExp(parent->get_parent())) {
      cerr << "$$$$$ Found Function call - lets handle its parameters." << endl;
      SgType* type = isSgExpression(n)->get_type();
      if (type && isSgArrayType(type))
         donothing=true;
   }
   if (isSgAddressOfOp(parent)) {
      // consider, e.g.
      // 	int x;
      // 	int* y = &x;
      // it is not necessary for x to be initialized
      donothing = true;
   }
   if (isSgArrayType( n -> get_type() ))
      donothing=true;

   if (donothing==false)
      variable_access_varref.push_back(n);
#endif
}

#endif



