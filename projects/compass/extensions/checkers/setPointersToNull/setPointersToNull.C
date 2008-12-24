// Set Pointers To Null
// Author: Mark Lewandowski, -422-3849
// Date: 19-September-2007
// reimplemented tps 23Dec 2008

#include "compass.h"

#ifndef COMPASS_SET_POINTERS_TO_NULL_H
#define COMPASS_SET_POINTERS_TO_NULL_H

using namespace std;

namespace CompassAnalyses
   { 
     namespace SetPointersToNull
        { 
        /*! \brief Set Pointers To Null: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
	       CheckerOutput(SgNode* node,const std::string & reason );
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
                    void run(SgNode* n){ this->traverse(n, postorder); }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_SET_POINTERS_TO_NULL_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2:tw=80

// Set Pointers To Null Analysis
// Author: Mark Lewandowski, -422-3849
// Date: 19-September-2007

#include "compass.h"
// #include "setPointersToNull.h"

namespace CompassAnalyses
   { 
     namespace SetPointersToNull
        { 
          const std::string checkerName      = "SetPointersToNull";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Set all dynamically allocated pointers to NULL after calling free().";
          const std::string longDescription  = "A simple yet effective way to avoid double-free and access-freed-memory vulnerabilities is to set pointers to NULL after they have been freed. Calling free() on a NULL pointer results in no action being taken by free(). Thus, it is recommended that freed pointers be set to NULL to help eliminate memory related vulnerabilities.";
        } //End of namespace SetPointersToNull.
   } //End of namespace CompassAnalyses.

CompassAnalyses::SetPointersToNull::
CheckerOutput::CheckerOutput ( SgNode* node,const std::string & reason  )
   : OutputViolationBase(node,checkerName,reason+shortDescription)
   {}

CompassAnalyses::SetPointersToNull::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["SetPointersToNull.YourParameter"]);


   }

void
CompassAnalyses::SetPointersToNull::Traversal::
visit(SgNode* node)
   { 
     SgInitializedName* init=NULL;
    SgFunctionCallExp* func  = isSgFunctionCallExp(node);
    if (func) {
      SgFunctionRefExp *fref = isSgFunctionRefExp(func->get_function());
      SgMemberFunctionRefExp *fmem = isSgMemberFunctionRefExp(func->get_function());
      std::string func_name_str("");
      if( fref ) func_name_str = fref->get_symbol()->get_name().getString();
      if( fmem ) func_name_str = fmem->get_symbol()->get_name().getString();
      if(func_name_str.compare("free") == 0) {
	//std::cerr << "found free" << endl;
	SgExprListExp* list = func->get_args();
	// check if args == init
	Rose_STL_Container<SgExpression*> plist = list->get_expressions();

	SgExpression* argument = *plist.begin();
	while (isSgUnaryOp(argument)!=NULL)
	  argument=isSgUnaryOp(argument)->get_operand();

	SgVariableSymbol* var =NULL;
	switch (argument->variantT()) {
	case V_SgVarRefExp : {
	  var = isSgVarRefExp(argument)->get_symbol(); break;
	}
	case V_SgArrowExp : {
	  SgExpression* ex = isSgArrowExp(argument)->get_rhs_operand();
	  if (isSgVarRefExp(ex))
	    var = isSgVarRefExp(ex)->get_symbol(); 
	  else 
	    cerr <<"1: setPointersToNULL : isSgVarRefExp not found in isSgArrowExp "<< endl;
	  break;
	}
	case V_SgDotExp : {
	  SgExpression* ex = isSgDotExp(argument)->get_rhs_operand();
	  if (isSgVarRefExp(ex))
	    var = isSgVarRefExp(ex)->get_symbol(); 
	  else 
	    cerr <<"1: setPointersToNULL : isSgVarRefExp not found in isSgDotExp "<< endl;
	  break;
	}
	case V_SgPointerDerefExp : {
	  SgExpression* ex = isSgPointerDerefExp(argument)->get_operand();
	  if (isSgVarRefExp(ex))
	    var = isSgVarRefExp(ex)->get_symbol(); 
	  else 
	    cerr <<"1: setPointersToNULL : isSgVarRefExp not found in isSgPointerDerefExp "<< endl;
	  break;
	}
	default: {
	  cerr << "1: Left hand side of (assign) free is unknown: " << argument->class_name()<<endl;
	}
	} // switch

      if (var)
	  init = var->get_declaration();
      else 
	cerr << " 1: No var -- Left hand side of (assign) free is unknown: " << argument->class_name()<<endl;

      }
    }

  if (!init) return;


  //std::cerr << "Found malloc ... " << std::endl;
  // traverse cfg within this function and find malloc with same init
  // if not found trigger error
  vector<FilteredCFGNode<IsDFAFilter> > worklist;
  vector<FilteredCFGNode<IsDFAFilter> > visited;
  // add this node to worklist and work through the outgoing edges
  FilteredCFGNode < IsDFAFilter > source =
    FilteredCFGNode < IsDFAFilter > (node->cfgForEnd());

  worklist.push_back(source);
  while (!worklist.empty()) {
    source = worklist.front();
    worklist.erase(worklist.begin());
    SgNode* next = source.getNode();

    SgAssignOp* assign = isSgAssignOp(next);
    SgInitializedName* initAssign=NULL;
    if (assign) {
      SgExpression*  expr = assign->get_rhs_operand();
      while (isSgUnaryOp(expr)!=NULL)
	expr=isSgUnaryOp(expr)->get_operand();
      SgIntVal* intval = isSgIntVal(expr);
      int value=-1;
      // value should == 0 (NULL)
      if (intval)
	value=intval->get_value();
      //cerr <<"assign found : intval = " << intval << "   value = " << value << endl;
      if (value==0) {
	SgVariableSymbol* var = NULL;
	switch (assign->get_lhs_operand()->variantT()) {
	case V_SgVarRefExp : {
	  var = isSgVarRefExp(assign->get_lhs_operand())->get_symbol(); break;
	}
	case V_SgArrowExp : {
	  SgExpression* ex = isSgArrowExp(assign->get_lhs_operand())->get_rhs_operand();
	  if (isSgVarRefExp(ex))
	    var = isSgVarRefExp(ex)->get_symbol(); 
	  else 
	    cerr <<"setPointersToNULL : isSgVarRefExp not found in isSgArrowExp "<< endl;
	  break;
	}
	case V_SgDotExp : {
	  SgExpression* ex = isSgDotExp(assign->get_lhs_operand())->get_rhs_operand();
	  if (isSgVarRefExp(ex))
	    var = isSgVarRefExp(ex)->get_symbol(); 
	  else 
	    cerr <<"setPointersToNULL : isSgVarRefExp not found in isSgDotExp "<< endl;
	  break;
	}
	case V_SgPointerDerefExp : {
	  SgExpression* ex = isSgPointerDerefExp(assign->get_lhs_operand())->get_operand();
	  if (isSgVarRefExp(ex))
	    var = isSgVarRefExp(ex)->get_symbol(); 
	  else 
	    cerr <<"setPointersToNULL : isSgVarRefExp not found in isSgPointerDerefExp "<< endl;
	  break;
	}
	default: {
	  cerr << "Left hand side of (assign) free is unknown: " << assign->get_lhs_operand()->class_name()<<endl;
	}
	} // switch
      if (var)
	  initAssign = var->get_declaration();
      else 
	cerr << " No var -- Left hand side of (assign) free is unknown: " << assign->get_lhs_operand()->class_name()<<endl;
	
      } // value == 0
      //cerr << "init == " << init << "  initAssign = " << initAssign << endl;
      if (initAssign==init) {
	// we have found the variable and its assigned to NULL
	//cerr << " match!!!!! " << endl;
	return;
      }
    }

    vector<FilteredCFGEdge < IsDFAFilter > > out_edges = source.outEdges();
    for (vector<FilteredCFGEdge <IsDFAFilter> >::const_iterator i = out_edges.begin(); i != out_edges.end(); ++i) {
      FilteredCFGEdge<IsDFAFilter> filterEdge = *i;
      FilteredCFGNode<IsDFAFilter> filterNode = filterEdge.target();
      if (find(visited.begin(), visited.end(), filterNode)==visited.end()) {
	worklist.push_back(filterNode);
	visited.push_back(filterNode);
      }
    }
  }

  SgNode* parent = node->get_parent();
  while (!isSgFunctionDeclaration(parent) && !isSgGlobal(parent)) 
    parent=parent->get_parent();
  std::string funcname="";
  if (isSgFunctionDeclaration(parent))
    funcname=isSgFunctionDeclaration(parent)->get_name();
  std::string reason="\tin function: "+funcname+"\t";
	 

  // if we reach this point, then we have not detected a var=NULL after free!
  output->addOutput(new CheckerOutput(node,reason));
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::SetPointersToNull::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::SetPointersToNull::Traversal(params, output);
}

extern const Compass::Checker* const setPointersToNullChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::SetPointersToNull::checkerName,
        CompassAnalyses::SetPointersToNull::shortDescription,
        CompassAnalyses::SetPointersToNull::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
