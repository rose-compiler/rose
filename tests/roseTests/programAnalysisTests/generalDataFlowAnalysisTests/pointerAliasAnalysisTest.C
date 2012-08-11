#include "rose.h"
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <ctype.h>
#include <boost/algorithm/string.hpp>
using namespace std;
#include "genericDataflowCommon.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include <AstTraversal.h>
#include "printAnalysisStates.h"
#include "pointerAliasAnalysis.h"

bool mFilter (CFGNode cfgn)
{
      SgNode *node = cfgn.getNode();
      switch (node->variantT())
      {
          //    case V_SgBasicBlock:
           //    return cfgn == node->cfgForBeginning()  || cfgn == node->cfgForBeginning();
          case V_SgAssignStatement:
          case V_SgAssignOp:
          case V_SgAssignInitializer:
               //return (cfgn == node->cfgForBeginning());
               return (cfgn == node->cfgForEnd());
          case V_SgConditionalExp:
               return (cfgn.getIndex() == 1);
          case V_SgConstructorInitializer:
          case V_SgFunctionCallExp:
               return (cfgn == node->cfgForBeginning());
               //return (cfgn == node->cfgForEnd());
          case V_SgReturnStmt:
               return (cfgn == node->cfgForEnd());
               //return true;
          default:
               return false;
      }
}

bool attrFilter (CFGNode cfgn)
{
    SgNode *node = cfgn.getNode();
    if(node->attributeExists("VFA_attribute"))
        return false;
    
return cfgn.isInteresting();
}

class Traversal : public AstPreOrderTraversal
   {
     public:
          Traversal() {}

          // A function called on each node in the traversal
          void preOrderVisit(SgNode* node){
            AstAttribute* newAttribute = new AstAttribute();
            ROSE_ASSERT(newAttribute != NULL);
            std::cout << node->class_name() << " " << node << std::endl;
            node->addNewAttribute("VFA_attribute",newAttribute);         
            //std::cout<<" Attribute exists:" << node->attributeExists("VFA_attribute") <<endl;
          }
};

class VirtualFunctionAnalysis // public UnstructuredPassIntraAnalysis{
{
    pointerAliasAnalysis* pl;
    SgProject* project;
    ClassHierarchyWrapper *classHierarchy;
    vector<SgExpression*> callSites;
    
public:
    VirtualFunctionAnalysis(SgProject *_pro, pointerAliasAnalysis* _pl) : project(_pro), pl(_pl) {
        classHierarchy = new ClassHierarchyWrapper(project);
    };
    void ignoreVirtualFunctionCalls();
//    void updatePointerAlias(pointerAliasAnalysis* _poal);
//    void visit(const Function& func, const DataflowNode& n, NodeState& state);
    
};

void VirtualFunctionAnalysis::ignoreVirtualFunctionCalls()
{
    Traversal* trav = new Traversal();
    callSites = SageInterface::querySubTree<SgExpression> (project, V_SgFunctionCallExp);
    vector<SgExpression*> constrs = SageInterface::querySubTree<SgExpression> (project, V_SgConstructorInitializer);
    callSites.insert(callSites.end(), constrs.begin(), constrs.end());
    unsigned int index;
        
    //Identify Virtual Function Calls in AST and annotate it
    for(index = 0; index < callSites.size(); index++) {
            std::vector<SgFunctionDeclaration *> funcs;
            CallTargetSet::getPropertiesForExpression(callSites[index], classHierarchy, funcs);
            //Virtual Function
            if(isSgFunctionCallExp(callSites[index]) && funcs.size() > 1 ){
                std::cout<<"VFC :"<<isSgFunctionCallExp(callSites[index])->unparseToString()<<endl;
                trav->traverse(callSites[index]);        
            }
    }
    generateDOT(*project);   
}

/*
void VirtualFunctionAnalysis::updatePointerAlias(pointerAliasAnalysis* _poal)
{
    pl = _poal;
}
*/
/*
void VirtualFunctionAnalysis::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
    FiniteVarsExprsProductLattice *lat = dynamic_cast<FiniteVarsExprsProductLattice *>(state.getLatticeAbove(pl)[0]);
    set<varID> allVars = lat->getAllVars();
     for (set<varID>::iterator i = allVars.begin(); i != allVars.end(); ++i)
        {
            Dbg::dbg << "Variable " << i->str(" ") << " ";
        } 
}
*/


int main(int argc, char *argv[])
{
    SgProject* project = frontend(argc,argv);

    //Initialize the project
    initAnalysis(project);
    Dbg::init("Pointer Alias analysis Test", ".", "index.html");
    
    liveDeadAnalysisDebugLevel = 0;
    analysisDebugLevel = 1;
    if (liveDeadAnalysisDebugLevel)
        {
          printf("*************************************************************\n");
          printf("*****************   Pointer Alias Analysis   *****************\n");
          printf("*************************************************************\n");
        }

     //LiveDeadVarsAnalysis ldva(project);
     //UnstructuredPassInterDataflow ciipd_ldva(&ldva);
     //ciipd_ldva.runAnalysis();

     VirtualFunctionAnalysis vfal(project, NULL); 
     vfal.ignoreVirtualFunctionCalls();

    //Build the call graph
    CallGraphBuilder cgb(project);
    cgb.buildCallGraph();
    SgIncidenceDirectedGraph* graph = cgb.getGraph();

    //Setup pointerAliasAnalysis and filter CFG nodes based on mFilter criteria
    pointerAliasAnalysis poal(NULL);
    //poal.filter = attrFilter;

    ContextInsensitiveInterProceduralDataflow inter_al(&poal, graph);
    inter_al.runAnalysis();
    
    //vfal.updatePointerAlias(&poal);
    //UnstructuredPassInterAnalysis upia(vfal);
    //upia.runAnalysis();

    Dbg::dotGraphGenerator (&poal);
return 0;
}


