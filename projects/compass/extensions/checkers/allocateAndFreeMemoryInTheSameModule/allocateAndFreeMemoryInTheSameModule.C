// Allocate And Free Memory In The Same Module At The Same Level Of Abstraction
// Author: Mark Lewandowski, -422-3849
// Date: 16-November-2007
// tps, rewrote Dec22-2008

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_ALLOCATE_AND_FREE_MEMORY_IN_THE_SAME_MODULE_AT_THE_SAME_LEVEL_OF_ABSTRACTION_H
#define COMPASS_ALLOCATE_AND_FREE_MEMORY_IN_THE_SAME_MODULE_AT_THE_SAME_LEVEL_OF_ABSTRACTION_H

using namespace std;

namespace CompassAnalyses
{ 
  namespace AllocateAndFreeMemoryInTheSameModule
  { 
    /*! \brief Allocate And Free Memory In The Same Module At The Same Level Of Abstraction: Add your description here 
     */

    extern const std::string checkerName;
    extern const std::string shortDescription;
    extern const std::string longDescription;

    // Specification of Checker Output Implementation
    class CheckerOutput: public Compass::OutputViolationBase
    { 
    public:
      CheckerOutput(SgNode* node, 
                      const std::string &);
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

// COMPASS_ALLOCATE_AND_FREE_MEMORY_IN_THE_SAME_MODULE_AT_THE_SAME_LEVEL_OF_ABSTRACTION_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Allocate And Free Memory In The Same Module At The Same Level Of Abstraction Analysis
// Author: Mark Lewandowski, -422-3849
// Date: 16-November-2007

#include "compass.h"
// #include "allocateAndFreeMemoryInTheSameModule.h"

namespace CompassAnalyses
{ 
  namespace AllocateAndFreeMemoryInTheSameModule
  { 
    const std::string checkerName      = "AllocateAndFreeMemoryInTheSameModule";

    // Descriptions should not include the newline character "\n".
    const std::string shortDescription = "Every malloc must be followed by a free.";
    const std::string longDescription  = "Allocating and freeing memory in different modules and levels of abstraction burdens the programmer with tracking the lifetime of that block of memory. This may cause confusion regarding when and if a block of memory has been allocated or freed, leading to programming defects such as double-free vulnerabilities, accessing freed memory, or writing to unallocated memory.To avoid these situations, it is recommended that memory be allocated and freed at the same level of abstraction, and ideally in the same code module.The affects of not following this recommendation are best demonstrated by an actual vulnerability. Freeing memory in different modules resulted in a vulnerability in MIT Kerberos 5 MITKRB5-SA-2004-002 . The problem was that the MIT Kerberos 5 code contained error-handling logic, which freed memory allocated by the ASN.1 decoders if pointers to the allocated memory were non-NULL. However, if a detectable error occured, the ASN.1 decoders freed the memory that they had allocated. When some library functions received errors from the ASN.1 decoders, they also attempted to free, causing a double-free vulnerability.";
  } //End of namespace AllocateAndFreeMemoryInTheSameModule.
} //End of namespace CompassAnalyses.

CompassAnalyses::AllocateAndFreeMemoryInTheSameModule::
CheckerOutput::CheckerOutput ( SgNode* node, const std::string & reason  )
  : OutputViolationBase(node,checkerName,reason+shortDescription)
{}

CompassAnalyses::AllocateAndFreeMemoryInTheSameModule::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{
  // Initialize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["AllocateAndFreeMemoryInTheSameModule.YourParameter"]);
}

void
CompassAnalyses::AllocateAndFreeMemoryInTheSameModule::Traversal::
visit(SgNode* node)
{ 
     
  SgAssignOp* aop          = isSgAssignOp(node);
  SgAssignInitializer* ain = isSgAssignInitializer(node);
  SgInitializedName* init=NULL;
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
      switch (aop->get_lhs_operand()->variantT()) {
      case V_SgVarRefExp : {
	var = isSgVarRefExp(aop->get_lhs_operand())->get_symbol(); break;
      }
      case V_SgArrowExp : {
	SgExpression* ex = isSgArrowExp(aop->get_lhs_operand())->get_lhs_operand();
	if (isSgVarRefExp(ex))
	  var = isSgVarRefExp(ex)->get_symbol(); 
	  else 
	    cerr <<"allocateAndFreeMemory : isSgVarRefExp not found in isSgArrowExp "<< endl;
	break;
      }
      case V_SgDotExp : {
	SgExpression* ex = isSgDotExp(aop->get_lhs_operand())->get_lhs_operand();
	if (isSgVarRefExp(ex))
	  var = isSgVarRefExp(ex)->get_symbol(); 
	  else 
	    cerr <<"allocateAndFreeMemory : isSgVarRefExp not found in isSgDotExp "<< endl;
	break;
      }
	case V_SgPointerDerefExp : {
	  SgExpression* ex = isSgPointerDerefExp(aop->get_lhs_operand())->get_operand();
	  if (isSgVarRefExp(ex))
	    var = isSgVarRefExp(ex)->get_symbol(); 
	  else 
	    cerr <<"allocateAndFreeMemory : isSgVarRefExp not found in isSgPointerDerefExp "<< endl;
	  break;
	}
      default: {
	cerr << "Left hand side of malloc is unknown: " << aop->get_lhs_operand()->class_name()<<endl;
      }
      }
      if (var)
	  init = var->get_declaration();
      else 
	cerr << " No var -- Left hand side of malloc is unknown: " << aop->get_lhs_operand()->class_name()<<endl;
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
	rhop_func->get_symbol()->get_name().getString().compare("calloc") == 0	) {
      init = isSgInitializedName(ain->get_parent());
    }
  }

  if (!init) return;

  //std::cerr << "Found malloc or calloc ... " << std::endl;
  // traverse cfg within this function and find free with same init
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
    SgFunctionCallExp* func  = isSgFunctionCallExp(next);
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
	if (plist.size()>1) return;
	SgExpression* argument = *plist.begin();
	while (isSgUnaryOp(argument)!=NULL)
	  argument=isSgUnaryOp(argument)->get_operand();
	SgVarRefExp* lhop = isSgVarRefExp(argument);
	SgInitializedName* initFree=NULL;
	if (lhop) {
	  SgVariableSymbol* var = lhop->get_symbol();
	  if (var)
	    initFree = var->get_declaration();
	}
	if (init==initFree) {
	  // found free for matching malloc
	  //std::cerr << "init match - return " << endl;
	  return;
	} else {
	  std::cerr << "init doesnt match - init: " << init << "  initFree: " << initFree << " " << argument->class_name() << endl;
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

  SgNode* parent = node->get_parent();
  while (!isSgFunctionDeclaration(parent) && !isSgGlobal(parent)) 
    parent=parent->get_parent();
  std::string funcname="";
  if (isSgFunctionDeclaration(parent))
    funcname=isSgFunctionDeclaration(parent)->get_name();
  std::string reason="\tin function: "+funcname+"\t";
	 

  // if we reach this point, then we have not detected a free for a malloc   
  output->addOutput(new CheckerOutput(node,reason));


} //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::AllocateAndFreeMemoryInTheSameModule::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::AllocateAndFreeMemoryInTheSameModule::Traversal(params, output);
}

extern const Compass::Checker* const allocateAndFreeMemoryInTheSameModuleChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
                                               CompassAnalyses::AllocateAndFreeMemoryInTheSameModule::checkerName,
					       CompassAnalyses::AllocateAndFreeMemoryInTheSameModule::shortDescription,
					       CompassAnalyses::AllocateAndFreeMemoryInTheSameModule::longDescription,
					       Compass::C | Compass::Cpp,
					       Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
					       run,
					       createTraversal);
