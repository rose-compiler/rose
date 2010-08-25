// Buffer Overflow Functions
// Author: Thomas Panas
// Date: 23-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_BUFFER_OVERFLOW_FUNCTIONS_H
#define COMPASS_BUFFER_OVERFLOW_FUNCTIONS_H

using namespace std;

namespace CompassAnalyses
   { 
     namespace BufferOverflowFunctions
        { 
        /*! \brief Buffer Overflow Functions: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(std::string problem, SgNode* node);
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
	            void violation(SgNode* node);

             };
        }
   }

// COMPASS_BUFFER_OVERFLOW_FUNCTIONS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Buffer Overflow Functions Analysis
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"
// #include "bufferOverflowFunctions.h"

namespace CompassAnalyses
{ 
  namespace BufferOverflowFunctions
  { 
    const std::string checkerName      = "BufferOverflowFunctions";

    // Descriptions should not include the newline character "\n".
    const std::string shortDescription = "Detects functions that may cause a buffer overflow.";
    const std::string longDescription  = "This checker detects functions that may cause an buffer overflow if used inpropriate. For more information, check the manual.";
  } //End of namespace BufferOverflowFunctions.
} //End of namespace CompassAnalyses.

CompassAnalyses::BufferOverflowFunctions::
CheckerOutput::CheckerOutput ( std::string problem, SgNode* node )
  : OutputViolationBase(node,checkerName,shortDescription+" "+problem)
{}

CompassAnalyses::BufferOverflowFunctions::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["BufferOverflowFunctions.YourParameter"]);

}

void
CompassAnalyses::BufferOverflowFunctions::Traversal::
violation(SgNode* node) {
  SgNode* parent = node->get_parent();
  while (!isSgFunctionDeclaration(parent) && !isSgGlobal(parent)) 
    parent=parent->get_parent();
  std::string funcname="";
  if (isSgFunctionDeclaration(parent))
    funcname=isSgFunctionDeclaration(parent)->get_name();
  std::string reason="\tin function: "+funcname+"\t";
  
  
  // if we reach this point, then we have not detected a free for a malloc   
  output->addOutput(new CheckerOutput(reason,node));
}

void
CompassAnalyses::BufferOverflowFunctions::Traversal::
visit(SgNode* node)
{ 

  SgAssignOp* aop          = isSgAssignOp(node);
  SgAssignInitializer* ain = isSgAssignInitializer(node);
  SgInitializedName* init=NULL;
  SgExprListExp* express = NULL;
  if (aop) {
    SgExpression*  expr = aop->get_rhs_operand();
    while (isSgUnaryOp(expr)!=NULL)
      expr=isSgUnaryOp(expr)->get_operand();
    SgFunctionCallExp* rhop_func_call = isSgFunctionCallExp(expr);
    if( !rhop_func_call ) return;
    SgFunctionRefExp* rhop_func = isSgFunctionRefExp(rhop_func_call->get_function());
    if( !rhop_func ) return;
    if (rhop_func->get_symbol()->get_name().getString().compare("malloc") == 0 ||
	//      	rhop_func->get_symbol()->get_name().getString().compare("realloc") == 0 ||
	rhop_func->get_symbol()->get_name().getString().compare("calloc") == 0 ) {
      SgVariableSymbol* var = NULL;
      express = rhop_func_call->get_args();
      switch (aop->get_lhs_operand()->variantT()) {
      case V_SgVarRefExp : {
	var = isSgVarRefExp(aop->get_lhs_operand())->get_symbol(); break;
      }
      case V_SgArrowExp : {
	SgExpression* ex = isSgArrowExp(aop->get_lhs_operand())->get_lhs_operand();
	if (isSgVarRefExp(ex))
	  var = isSgVarRefExp(ex)->get_symbol(); 
	  else 
	    cerr <<"bufferOverflow : isSgVarRefExp not found in isSgArrowExp "<< endl;
	break;
      }
      case V_SgDotExp : {
	SgExpression* ex = isSgDotExp(aop->get_lhs_operand())->get_lhs_operand();
	if (isSgVarRefExp(ex))
	  var = isSgVarRefExp(ex)->get_symbol(); 
	  else 
	    cerr <<"bufferOverflow : isSgVarRefExp not found in isSgDotExp "<< endl;
	break;
      }
	case V_SgPointerDerefExp : {
	  SgExpression* ex = isSgPointerDerefExp(aop->get_lhs_operand())->get_operand();
	  if (isSgVarRefExp(ex))
	    var = isSgVarRefExp(ex)->get_symbol(); 
	  else 
	    cerr <<"bufferOverflow : isSgVarRefExp not found in isSgPointerDerefExp "<< endl;
	  break;
	}
      default: {
	cerr << "  bufferoverflow : Left hand side of malloc is unknown: " << aop->get_lhs_operand()->class_name()<<endl;
      }
      }
      if (var) 
	  init = var->get_declaration();
      else 
	cerr << "  bufferoverflow : No var -- Left hand side of malloc is unknown: " << aop->get_lhs_operand()->class_name()<<endl;
    }
  }

  if (ain) {
    SgExpression*  expr = ain->get_operand();
    while (isSgUnaryOp(expr)!=NULL)
      expr=isSgUnaryOp(expr)->get_operand();
    SgFunctionCallExp* rhop_func_call = isSgFunctionCallExp(expr);
    if( !rhop_func_call ) return;
    SgFunctionRefExp* rhop_func = isSgFunctionRefExp(rhop_func_call->get_function());
    if( !rhop_func ) return;
    if (rhop_func->get_symbol()->get_name().getString().compare("malloc") == 0 || 
	rhop_func->get_symbol()->get_name().getString().compare("calloc") == 0 	) {
      init = isSgInitializedName(ain->get_parent());
    } else {
      //      cerr << " Bufferoverflow :: found FuncRefExp that is not malloc : " 
      //	   << rhop_func->get_symbol()->get_name().getString() << endl;
    }
    express = rhop_func_call->get_args();
  }

  if (!init) return;
  if (!express) return;

  unsigned long arrayInit = 0;
  Rose_STL_Container<SgExpression*> exprList = express->get_expressions();
  SgExpression* expr = *(exprList.begin());
  if (isSgUnsignedLongVal(expr)) {
    arrayInit = (isSgUnsignedLongVal(expr))->get_value();
  }  else {
    cerr << " BufferOverflowFunctions :: " << 
      "dont know the value of the malloc allocation : " <<
      expr->class_name() << endl;
  }
  // query down to find sizeof operation -- we need to know the type!
  Rose_STL_Container<SgNode*> sizeofOp = 
    NodeQuery::querySubTree(expr, V_SgSizeOfOp);
  // stop if we have a node with unknown typesize
  if (sizeofOp.size()!=1)
    return;
  SgUnsignedLongLongIntVal* valType = isSgUnsignedLongLongIntVal((*(sizeofOp.begin()))->get_parent());
  if (valType==NULL)
    cerr << "    buffer overflow - problem with parent : " << 
      (*(sizeofOp.begin()))->get_parent()->class_name() << endl;
  unsigned long typeSize = 0;

  if (valType)
    typeSize = valType->get_value();
  if (typeSize==0)
    return;
  arrayInit=arrayInit/typeSize;

  cerr << "   Bufferoverflow :: Malloc found " <<
    init->get_name().getString() <<"  with size : " << RoseBin_support::ToString(arrayInit) << endl;
  



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

    SgPntrArrRefExp* pntr = isSgPntrArrRefExp(next);
    if (pntr) {
      SgVarRefExp* leftVar = isSgVarRefExp(pntr->get_lhs_operand());
      SgInitializedName* initVar=NULL;
      if (leftVar) {
	SgVariableSymbol* var = leftVar->get_symbol();
	if (var)
	  initVar = var->get_declaration();
      }
      if (initVar!=init)
	continue;
      switch (pntr->get_rhs_operand()->variantT()) {
      case V_SgIntVal: {
	SgIntVal* val = isSgIntVal(pntr->get_rhs_operand());
	unsigned int value = val->get_value();
	cerr << "  bufferoverflow : comparing " << value << " " 
             << arrayInit << "  violation? : " << (value>=arrayInit) << endl;
        if (value>=arrayInit) {
	  violation(next);
	}
	break;
      }
      case V_SgVarRefExp : {
	// we would need to track this variable down
	// in order to find out what the value is e.g. arr[i]
	break;
      }
      default: {
	cerr << "  bufferoverflow : cant determine right hand side of var use : " << 
	  pntr->get_rhs_operand()->class_name() << endl;
      }
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

  




  // this implementation is deprecated as it is used in the forbidden functions checker.
  // the new implementation looks for real buffer overflows.
#if 0
  // Implement your traversal here.  
  if (isSgFunctionCallExp(node)) {
    SgFunctionCallExp* fcexp = isSgFunctionCallExp( node);
    std::vector<SgNode*> sgExprListVec = fcexp->get_traversalSuccessorContainer();
    for (unsigned int i=0; i< sgExprListVec.size() ; i++) {
      if (isSgFunctionRefExp(sgExprListVec[i])) {
        SgFunctionRefExp* funcRefExp = (SgFunctionRefExp*) sgExprListVec[i];
        SgFunctionSymbol* funcSymbol = funcRefExp->get_symbol();
        std::string name = funcSymbol->get_name().str();
        //                  std::cout << " --- found SgFunctionRefExp: " << name << std::endl;
        if (name.compare("sprintf")==0) {
          std::string problem = "found sprintf(). Use snprintf instead. ";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("scanf")==0) {
          std::string problem = "found scanf().";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("sscanf")==0) {
          std::string problem = "found sscanf().";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("gets")==0) {
          std::string problem = "found gets(). Use fgets() instead.";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("strcpy")==0) {
          std::string problem = "found strcpy(). Use strncpy() instead.";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("_mbscpy")==0) {
          std::string problem = "found _mbscpy().";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("lstrcat")==0) {
          std::string problem = "found lstrcat().";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("memcpy")==0) {
          std::string problem = "found memcpy(). Check for buffer overflow when copying to destination.";
          output->addOutput(new CheckerOutput(problem, node));
        }
        if (name.compare("strcat")==0) {
          std::string problem = "found strcat(). Use strncat() instead.";
          output->addOutput(new CheckerOutput(problem, node));
        }
      }
    }
  }
#endif
} //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::BufferOverflowFunctions::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::BufferOverflowFunctions::Traversal(params, output);
}

extern const Compass::Checker* const bufferOverflowFunctionsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::BufferOverflowFunctions::checkerName,
        CompassAnalyses::BufferOverflowFunctions::shortDescription,
        CompassAnalyses::BufferOverflowFunctions::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
