// Possibly Replicated Variables
// Author: Jeremiah J. Willcock
// Date: 07-January-2008

#include "compass.h"

#ifndef COMPASS_POSSIBLY_REPLICATED_VARIABLES_H
#define COMPASS_POSSIBLY_REPLICATED_VARIABLES_H

namespace CompassAnalyses
   { 
     namespace PossiblyReplicatedVariables
        { 
        /*! \brief Possibly Replicated Variables: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node, bool isSharable);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
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
                    void run(SgNode* n);
             };
        }
   }

// COMPASS_POSSIBLY_REPLICATED_VARIABLES_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Possibly Replicated Variables Analysis
// Author: Jeremiah J. Willcock
// Date: 07-January-2008

#include "compass.h"
// #include "possiblyReplicatedVariables.h"
#include <AstInterface.h>
#include <StmtInfoCollect.h>
#include <ReachingDefinition.h>
#include <DefUseChain.h>
#include <DirectedGraph.h>
#include "DependenceGraph.h"
#include "SlicingInfo.h"
#include "CreateSlice.h"
#include "DominatorTree.h"
#include "CreateSliceSet.h"

namespace CompassAnalyses
   { 
     namespace PossiblyReplicatedVariables
        { 
          const std::string checkerName      = "PossiblyReplicatedVariables";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Find variables which are independent of MPI process rank";
          const std::string longDescription  = "Long description not written yet!";
        } //End of namespace PossiblyReplicatedVariables.
   } //End of namespace CompassAnalyses.

static std::string getErrorMessage(SgNode* node, bool isSharable) {
  std::string result = "Variable '";
  ROSE_ASSERT (isSgInitializedName(node));
  result += isSgInitializedName(node)->get_name().getString();
  result += "' ";
  if (isSharable) {
    result += "is independent of MPI rank";
  } else {
    result += "cannot be shared among MPI ranks";
  }
  return result;
}

CompassAnalyses::PossiblyReplicatedVariables::
CheckerOutput::CheckerOutput ( SgNode* node, bool isSharable )
   : OutputViolationBase(node,checkerName,getErrorMessage(node, isSharable))
   {}

CompassAnalyses::PossiblyReplicatedVariables::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["PossiblyReplicatedVariables.YourParameter"]);


   }

using namespace DominatorTreesAndDominanceFrontiers;
using namespace std;

static bool isMPI_Comm_rank(SgNode* n) {
  SgFunctionCallExp* fc = isSgFunctionCallExp(n);
  if (!fc) return false;
  SgFunctionRefExp* f = isSgFunctionRefExp(fc->get_function());
  if (!f) return false;
  SgFunctionSymbol* sym = f->get_symbol();
  SgFunctionDeclaration* decl = sym->get_declaration();
  if (decl->get_name().getString() != "MPI_Comm_rank") {
    return false;
  }
  // FIXME: check for mpi.h
  return true;
}

static SgExpression* getSecondArgOfFunctionCall(SgFunctionCallExp* fc) {
  SgExprListExp* args = fc->get_args();
  ROSE_ASSERT (args->get_expressions().size() >= 2);
  return args->get_expressions()[1];
}

static bool isCommRankCall(DependenceNode* dn) {
  return dn->getType() == DependenceNode::ACTUALRETURN && isMPI_Comm_rank(dn->getSgNode());
}

static std::vector<DependenceNode*>
getSuccessorsOfType(SystemDependenceGraph* sdg, DependenceNode* dn, SystemDependenceGraph::EdgeType t) {
  std::vector<DependenceNode*> result;
  std::set<SimpleDirectedGraphNode*> succs = dn->getSuccessors();
  for (std::set<SimpleDirectedGraphNode*>::const_iterator
         i = succs.begin();
       i != succs.end(); ++i) {
    DependenceNode* tgt = dynamic_cast<DependenceNode*>(*i);
    ROSE_ASSERT (tgt);
    bool isCorrectType = sdg->edgeExists(dn, tgt, t);
    if (!isCorrectType) {
      continue;
    }
    result.push_back(tgt);
  }
  return result;
}

static std::vector<DependenceNode*>
getPredecessorsOfType(SystemDependenceGraph* sdg, DependenceNode* dn, SystemDependenceGraph::EdgeType t) {
  std::vector<DependenceNode*> result;
  std::set<SimpleDirectedGraphNode*> preds = dn->getPredecessors();
  for (std::set<SimpleDirectedGraphNode*>::const_iterator
         i = preds.begin();
       i != preds.end(); ++i) {
    DependenceNode* src = dynamic_cast<DependenceNode*>(*i);
    ROSE_ASSERT (src);
    bool isCorrectType = sdg->edgeExists(src, dn, t);
    if (!isCorrectType) {
      continue;
    }
    result.push_back(src);
  }
  return result;
}

static DependenceNode*
getParamOutOfCommRank(SystemDependenceGraph* sdg, DependenceNode* call) {
  if (!isCommRankCall(call)) return 0;
  std::vector<DependenceNode*> preds = getPredecessorsOfType(sdg, call, SystemDependenceGraph::SUMMARY);
  DependenceNode* actualInNode = 0;
  SgFunctionCallExp* calln = isSgFunctionCallExp(call->getSgNode());
  for (std::vector<DependenceNode*>::const_iterator
         i = preds.begin();
       i != preds.end(); ++i) {
    DependenceNode* src = *i;
    SgNode* srcn = src->getSgNode();
    ROSE_ASSERT (calln);
    if (srcn != getSecondArgOfFunctionCall(calln)) {
      continue;
    }
    ROSE_ASSERT (src->getType() == DependenceNode::ACTUALIN);
    actualInNode = src;
    break;
  }
  std::vector<DependenceNode*> summaryEdgesOut = getSuccessorsOfType(sdg, actualInNode, SystemDependenceGraph::SUMMARY);
  for (std::vector<DependenceNode*>::const_iterator i = summaryEdgesOut.begin();
       i != summaryEdgesOut.end(); ++i) {
    if ((*i)->getType() != DependenceNode::ACTUALOUT) continue;
    return *i;
  }
  ROSE_ASSERT (!"Did not find ACTUALOUT from MPI_Comm_rank call");
}

static std::vector<DependenceNode*>
getCommRankSecondArgs(SystemDependenceGraph* sdg) {
  std::vector<DependenceNode*> result;
  std::set<SimpleDirectedGraphNode*> nodes = sdg->getNodes();
  for (std::set<SimpleDirectedGraphNode*>::const_iterator i = nodes.begin();
       i != nodes.end(); ++i) {
    DependenceNode* dn = dynamic_cast<DependenceNode*>(*i);
    ROSE_ASSERT (dn);
    DependenceNode* param = getParamOutOfCommRank(sdg, dn);
    if (param) {
      // cout << "Found second arg " << param->getSgNode()->unparseToString() << endl;
      result.push_back(param);
    }
  }
  return result;
}

static void findSdgSuccessorsHelper(SystemDependenceGraph* sdg, std::set<DependenceNode*>& done, DependenceNode* start) {
  if (done.find(start) != done.end()) return;
  done.insert(start);
  std::vector<DependenceNode*> succs1 = getSuccessorsOfType(sdg, start, SystemDependenceGraph::DATA);
  std::vector<DependenceNode*> succs2 = getSuccessorsOfType(sdg, start, SystemDependenceGraph::CONTROL);
  std::vector<DependenceNode*> succs3 = getSuccessorsOfType(sdg, start, SystemDependenceGraph::SUMMARY);
  std::vector<DependenceNode*> succs4; // = getSuccessorsOfType(sdg, start, SystemDependenceGraph::PARAMETER_IN);
  std::vector<DependenceNode*> succs5; // = getSuccessorsOfType(sdg, start, SystemDependenceGraph::PARAMETER_OUT);
  std::vector<DependenceNode*> succs6;
  if (start->getType() == DependenceNode::ACTUALRETURN) {
    succs6 = getPredecessorsOfType(sdg, start, SystemDependenceGraph::SUMMARY);
  }
  succs1.insert(succs1.end(), succs2.begin(), succs2.end());
  succs1.insert(succs1.end(), succs3.begin(), succs3.end());
  succs1.insert(succs1.end(), succs4.begin(), succs4.end());
  succs1.insert(succs1.end(), succs5.begin(), succs5.end());
  succs1.insert(succs1.end(), succs6.begin(), succs6.end());
  for (std::vector<DependenceNode*>::const_iterator i = succs1.begin();
       i != succs1.end(); ++i) {
    DependenceNode* tgt = *i;
    ROSE_ASSERT (tgt);
    findSdgSuccessorsHelper(sdg, done, tgt);
  }
}

static std::set<DependenceNode*> getRankDependentNodes(SystemDependenceGraph* sdg) {
  std::vector<DependenceNode*> secondArgs = getCommRankSecondArgs(sdg);
  std::set<DependenceNode*> result;
  for (std::vector<DependenceNode*>::const_iterator i = secondArgs.begin();
       i != secondArgs.end(); ++i) {
    findSdgSuccessorsHelper(sdg, result, *i);
  }
  return result;
}

static std::set<SgInitializedName*> getPossiblySharableVars(SgProject* proj) {
  std::set<SgInitializedName*> result;
  std::vector<SgNode*> allVars = NodeQuery::querySubTree(proj, V_SgInitializedName);
  for (std::vector<SgNode*>::const_iterator i = allVars.begin(); i != allVars.end(); ++i) {
    SgInitializedName* in = isSgInitializedName(*i);
    ROSE_ASSERT (in);
#if 0
    SgType* t = in->get_type()->stripType(SgType::STRIP_TYPEDEF_TYPE || SgType::STRIP_MODIFIER_TYPE);
    if (!isSgPointerType(t) && !isSgArrayType(t)) {
      continue;
    }
#endif
    if (!isSgVariableDeclaration(in->get_parent())) continue;
    if (isSgClassDefinition(in->get_parent()->get_parent())) continue;
    result.insert(in);
  }
  return result;
}

static void removeUnsharableForExpr(SgExpression* e,
                                    std::set<SgInitializedName*>& sharable) {
  if (isSgVarRefExp(e)) {
    sharable.erase(isSgVarRefExp(e)->get_symbol()->get_declaration());
  } else {
    std::vector<SgNode*> children = e->get_traversalSuccessorContainer();
    for (unsigned int i = 0; i < children.size(); ++i) {
      if (isSgExpression(children[i])) {
        removeUnsharableForExpr(isSgExpression(children[i]), sharable);
      }
    }
  }
}

static void removeUnsharableVariables(const std::set<DependenceNode*>& rankDependent,
                                      std::set<SgInitializedName*>& sharable) {
  for (std::set<DependenceNode*>::const_iterator i = rankDependent.begin();
       i != rankDependent.end(); ++i) {
    if ((*i)->getType() != DependenceNode::SGNODE) continue;
    SgNode* n = (*i)->getSgNode();
    if (!isSgExpression(n)) continue;
    removeUnsharableForExpr(isSgExpression(n), sharable);
  }
}

static SystemDependenceGraph* makeSDG(SgProject* proj) {
  // Copied (and hacked) from src/midend/programAnalysis/staticInterproceduralSlicing/testSDG.C
  std::vector<InterproceduralInfo*> ip;
#ifdef NEWDU
  // Create the global def-use analysis
  EDefUse *defUseAnalysis=new EDefUse(proj);
  if (defUseAnalysis->run(false)==0)
  {
    std::cerr<<"DFAnalysis failed!"<<endl;
  }
#endif
  string outputFileName=proj->get_fileList().front()->get_sourceFileNameWithoutPath();
  SystemDependenceGraph *sdg = new SystemDependenceGraph;
  // for all function-declarations in the AST
  vector < SgNode * >functionDeclarations = NodeQuery::querySubTree(proj, V_SgFunctionDeclaration);
  for (vector < SgNode * >::iterator i = functionDeclarations.begin(); i != functionDeclarations.end(); i++)
  {
    ControlDependenceGraph *cdg;
    DataDependenceGraph *ddg;
    //	FunctionDependenceGraph * pdg;
    InterproceduralInfo *ipi;
    SgFunctionDeclaration *fDec = isSgFunctionDeclaration(*i);
    ROSE_ASSERT(fDec != NULL);
    // CI (01/08/2007): A missing function definition is an indicator to a 
    // 
    // 
    // librarycall. 
    // * An other possibility would be a programmer-mistake, which we
    // don't treat at this point.  // I assume librarycall
    if (fDec->get_definition() == NULL)
    {
      //			if (fDec->get_file_info()->isCompilerGenerated()) continue;
      // treat librarycall -> iterprocedualInfo must be created...
      // make all call-parameters used and create a function stub for
      // the graph
      ipi=new InterproceduralInfo(fDec);
      ipi->addExitNode(fDec);
      sdg->addInterproceduralInformation(ipi);
      sdg->createSafeConfiguration(fDec);
      ip.push_back(ipi);
      // This is somewhat a waste of memory and a more efficient approach might generate this when needed, but at the momenent everything is created...
    }
    else
    {
      // get the control depenence for this function
      ipi=new InterproceduralInfo(fDec);
      ROSE_ASSERT(ipi != NULL);
      // get control dependence for this function defintion
      cdg = new ControlDependenceGraph(fDec->get_definition(), ipi);
      cdg->computeInterproceduralInformation(ipi);
      // get the data dependence for this function
#ifdef NEWDU
      ddg = new DataDependenceGraph(fDec->get_definition(), defUseAnalysis,ipi);
#else
      ddg = new DataDependenceGraph(fDec->get_definition(), ipi);
#endif
      sdg->addFunction(cdg,ddg);
      sdg->addInterproceduralInformation(ipi);
      ip.push_back(ipi);
    }   
    // else if (fD->get_definition() == NULL)
  }
  // now all function-declarations have been process as well have all function-definitions
  // perform interproceduralAnalysys
  sdg->performInterproceduralAnalysis();
  set<SgNode*> preserve;
  if (sdg->getMainFunction()!=NULL) {
    preserve.insert(sdg->getMainFunction());
    sdg->cleanUp(preserve);
  }
  // string filename = (outputFileName)+".final.sdg.dot";
  // sdg->writeDot((char *)filename.c_str());
  return sdg;
}

void
CompassAnalyses::PossiblyReplicatedVariables::Traversal::
run(SgNode* node)
   { 
     ROSE_ASSERT (isSgProject(node));
     SgProject* sageProject = isSgProject(node);
     SystemDependenceGraph* sdg = makeSDG(sageProject);
     std::set<DependenceNode*> privateNodes = getRankDependentNodes(sdg);
     for (std::set<DependenceNode*>::const_iterator i = privateNodes.begin();
          i != privateNodes.end(); ++i) {
       if ((*i)->getType() != DependenceNode::SGNODE) continue;
       SgNode* n = (*i)->getSgNode();
       cout << "Found private node " << n->class_name() << ": " << n->unparseToString() << endl;
     }
     std::set<SgInitializedName*> possiblySharable = getPossiblySharableVars(sageProject);
     std::set<SgInitializedName*> sharable = possiblySharable;
     removeUnsharableVariables(privateNodes, sharable);
     std::set<SgInitializedName*> privateVars;
     std::set_difference(possiblySharable.begin(), possiblySharable.end(), sharable.begin(), sharable.end(), std::inserter(privateVars, privateVars.end()));
     for (std::set<SgInitializedName*>::const_iterator i = sharable.begin();
         i != sharable.end(); ++i) {
       output->addOutput(new CheckerOutput(*i, true));
     }
     for (std::set<SgInitializedName*>::const_iterator i = privateVars.begin();
         i != privateVars.end(); ++i) {
       output->addOutput(new CheckerOutput(*i, false));
     }
   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::PossiblyReplicatedVariables::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

extern const Compass::Checker* const possiblyReplicatedVariablesChecker =
  new Compass::Checker(
        CompassAnalyses::PossiblyReplicatedVariables::checkerName,
        CompassAnalyses::PossiblyReplicatedVariables::shortDescription,
        CompassAnalyses::PossiblyReplicatedVariables::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run);
